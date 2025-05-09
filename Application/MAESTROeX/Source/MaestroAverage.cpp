
#include <Maestro.H>
#include <Maestro_F.H>

using namespace amrex;

// Given a multifab of data (phi), average down to a base state quantity, phibar.
// If we are in plane-parallel, the averaging is at constant height.
// If we are spherical, { the averaging is done at constant radius.

void Maestro::Average(const Vector<MultiFab>& phi, BaseState<Real>& phibar,
                      int comp) {
    // timer for profiling
    BL_PROFILE_VAR("Maestro::Average()", Average);

    const auto nr_irreg = base_geom.nr_irreg;

    phibar.setVal(0.0);

    if (!spherical) {
        // planar case

        // phibar is dimensioned to "max_radial_level" so we must mimic that for phisum
        // so we can simply swap this result with phibar
        BaseState<Real> phisum(base_geom.max_radial_level + 1,
                               base_geom.nr_fine);
        phisum.setVal(0.0);
        auto phisum_arr = phisum.array();

        // this stores how many cells there are laterally at each level
        BaseState<int> ncell_s(base_geom.max_radial_level + 1);
        auto ncell = ncell_s.array();

        // loop is over the existing levels (up to finest_level)
        for (int lev = 0; lev <= finest_level; ++lev) {
            // Get the index space of the domain
            const Box domainBox = geom[lev].Domain();

            // compute number of cells at any given height for each level
            if (AMREX_SPACEDIM == 2) {
                ncell(lev) = domainBox.bigEnd(0) + 1;
            } else if (AMREX_SPACEDIM == 3) {
                ncell(lev) =
                    (domainBox.bigEnd(0) + 1) * (domainBox.bigEnd(1) + 1);
            }

            // Loop over boxes (make sure mfi takes a cell-centered multifab as an argument)
#ifdef _OPENMP
#pragma omp parallel if (!system::regtest_reduction)
#endif
            for (MFIter mfi(phi[lev], TilingIfNotGPU()); mfi.isValid(); ++mfi) {
                // Get the index space of the valid region
                const Box& tilebox = mfi.tilebox();

                const Array4<const Real> phi_arr = phi[lev].array(mfi, comp);

#ifdef AMREX_USE_CUDA
                // Atomic::Add is non-deterministic on the GPU. If this flag is true,
                // run on the CPU instead
                bool launched;
                if (deterministic_nodal_solve) {
                    launched = !Gpu::notInLaunchRegion();
                    // turn off GPU
                    if (launched) Gpu::setLaunchRegion(false);
                }
#endif

                ParallelFor(tilebox, [=] AMREX_GPU_DEVICE(int i, int j, int k) {
                    int r = AMREX_SPACEDIM == 2 ? j : k;
                    amrex::HostDevice::Atomic::Add(&(phisum_arr(lev, r)),
                                                   phi_arr(i, j, k));
                });

#ifdef AMREX_USE_CUDA
                if (deterministic_nodal_solve) {
                    // turn GPU back on
                    if (launched) Gpu::setLaunchRegion(true);
                }
#endif
            }
        }

        // reduction over boxes to get sum
        ParallelDescriptor::ReduceRealSum(
            phisum.dataPtr(),
            (base_geom.max_radial_level + 1) * base_geom.nr_fine);

        // divide phisum by ncell so it stores "phibar"
        for (int lev = 0; lev <= finest_level; ++lev) {
            for (auto i = 1; i <= base_geom.numdisjointchunks(lev); ++i) {
                const int lo = base_geom.r_start_coord(lev, i);
                const int hi = base_geom.r_end_coord(lev, i);
                ParallelFor(hi - lo + 1, [=] AMREX_GPU_DEVICE(int j) {
                    int r = j + lo;
                    phisum_arr(lev, r) /= ncell(lev);
                });
                Gpu::synchronize();
            }
        }

        RestrictBase(phisum, true);
        FillGhostBase(phisum, true);

        // swap pointers so phibar contains the computed average
        phisum.swap(phibar);

    } else if (spherical && use_exact_base_state) {
        // spherical case with uneven base state spacing

        // phibar is dimensioned to "max_radial_level" so we must mimic that for phisum
        // so we can simply swap this result with phibar
        BaseState<Real> phisum(base_geom.max_radial_level + 1,
                               base_geom.nr_fine);
        phisum.setVal(0.0);
        auto phisum_arr = phisum.array();

        // this stores how many cells there are at each level
        BaseState<int> ncell_s(base_geom.max_radial_level + 1,
                               base_geom.nr_fine);
        auto ncell = ncell_s.array();

        // loop is over the existing levels (up to finest_level)
        for (int lev = 0; lev <= finest_level; ++lev) {
// Loop over boxes (make sure mfi takes a cell-centered multifab as an argument)
#ifdef _OPENMP
#pragma omp parallel if (!system::regtest_reduction)
#endif
            for (MFIter mfi(phi[lev], TilingIfNotGPU()); mfi.isValid(); ++mfi) {
                // Get the index space of the valid region
                const Box& tilebox = mfi.tilebox();

                const Array4<const int> cc_to_r = cell_cc_to_r[lev].array(mfi);
                const Array4<const Real> phi_arr = phi[lev].array(mfi, comp);

                ParallelFor(tilebox, [=] AMREX_GPU_DEVICE(int i, int j, int k) {
                    auto index = cc_to_r(i, j, k);

                    amrex::HostDevice::Atomic::Add(&(phisum_arr(lev, index)),
                                                   phi_arr(i, j, k));
                    amrex::HostDevice::Atomic::Add(&(ncell(lev, index)), 1);
                });
            }
        }

        // reduction over boxes to get sum
        ParallelDescriptor::ReduceRealSum(
            phisum.dataPtr(),
            (base_geom.max_radial_level + 1) * base_geom.nr_fine);
        ParallelDescriptor::ReduceIntSum(
            ncell.dataPtr(),
            (base_geom.max_radial_level + 1) * base_geom.nr_fine);

        // divide phisum by ncell so it stores "phibar"
        for (int lev = 0; lev <= base_geom.max_radial_level; ++lev) {
            for (int r = 0; r < base_geom.nr_fine; ++r) {
                if (ncell(lev, r) > 0) {
                    phisum_arr(lev, r) /= Real(ncell(lev, r));
                } else {
                    // keep value constant if it is outside the cutoff coords
                    phisum_arr(lev, r) = phisum_arr(lev, r - 1);
                }
            }
        }

        RestrictBase(phisum, true);
        FillGhostBase(phisum, true);

        // swap pointers so phibar contains the computed average
        phisum.swap(phibar);
    } else {
        // spherical case with even base state spacing

        // For spherical, we construct a 1D array at each level, phisum, that has space
        // allocated for every possible radius that a cell-center at each level can
        // map into.  The radial locations have been precomputed and stored in radii.
        BaseState<Real> phisum_s(finest_level + 1, nr_irreg + 2);
        auto phisum = phisum_s.array();
        phisum_s.setVal(0.0);
        BaseState<Real> radii_s(finest_level + 1, nr_irreg + 3);
        auto radii = radii_s.array();
        BaseState<int> ncell_s(finest_level + 1, nr_irreg + 2);
        auto ncell = ncell_s.array();
        ncell_s.setVal(0);

        const auto& center_p = center;

        const int fine_lev = finest_level + 1;

        // radii contains every possible distance that a cell-center at the finest
        // level can map into
        for (int lev = 0; lev <= finest_level; ++lev) {
            // Get the index space of the domain
            const auto dx = geom[lev].CellSizeArray();

            ParallelFor(nr_irreg + 1, [=] AMREX_GPU_DEVICE(int r) {
                radii(lev, r + 1) = std::sqrt(0.75 + 2.0 * Real(r)) * dx[0];
            });
            Gpu::synchronize();

            radii(lev, nr_irreg + 2) = 1.e99;
            radii(lev, 0) = 0.0;
        }

        // loop is over the existing levels (up to finest_level)
        for (int lev = finest_level; lev >= 0; --lev) {
            // Get the grid size of the domain
            const auto dx = geom[lev].CellSizeArray();
            const auto prob_lo = geom[lev].ProbLoArray();

            // get references to the MultiFabs at level lev
            const MultiFab& phi_mf = phi[lev];

            // create mask assuming refinement ratio = 2
            int finelev = lev + 1;
            if (lev == finest_level) {
                finelev = finest_level;
            }

            const BoxArray& fba = phi[finelev].boxArray();
            const iMultiFab& mask = makeFineMask(phi_mf, fba, IntVect(2));

            // Loop over boxes (make sure mfi takes a cell-centered multifab as an argument)
#ifdef _OPENMP
#pragma omp parallel if (!system::regtest_reduction)
#endif
            for (MFIter mfi(phi_mf, TilingIfNotGPU()); mfi.isValid(); ++mfi) {
                // Get the index space of the valid region
                const Box& tilebox = mfi.tilebox();

                const Array4<const int> mask_arr = mask.array(mfi);
                const Array4<const Real> phi_arr = phi[lev].array(mfi, comp);

                bool use_mask = !(lev == fine_lev - 1);

#ifdef AMREX_USE_CUDA
                // Atomic::Add is non-deterministic on the GPU. If this flag is true,
                // run on the CPU instead
                bool launched;
                if (deterministic_nodal_solve) {
                    launched = !Gpu::notInLaunchRegion();
                    // turn off GPU
                    if (launched) Gpu::setLaunchRegion(false);
                }
#endif

                ParallelFor(tilebox, [=] AMREX_GPU_DEVICE(int i, int j, int k) {
                    Real x = prob_lo[0] + (Real(i) + 0.5) * dx[0] - center_p[0];
                    Real y = prob_lo[1] + (Real(j) + 0.5) * dx[1] - center_p[1];
                    Real z = prob_lo[2] + (Real(k) + 0.5) * dx[2] - center_p[2];

                    // make sure the cell isn't covered by finer cells
                    bool cell_valid = true;
                    if (use_mask) {
                        if (mask_arr(i, j, k) == 1) {
                            cell_valid = false;
                        }
                    }

                    if (cell_valid) {
                        // compute distance to center
                        Real radius = sqrt(x * x + y * y + z * z);

                        // figure out which radii index this point maps into
                        auto index = (int)amrex::Math::round(
                            ((radius / dx[0]) * (radius / dx[0]) - 0.75) / 2.0);

                        // due to roundoff error, need to ensure that we are in the proper radial bin
                        if (index < nr_irreg) {
                            if (amrex::Math::abs(radius -
                                                 radii(lev, index + 1)) >
                                amrex::Math::abs(radius -
                                                 radii(lev, index + 2))) {
                                index++;
                            }
                        }

                        amrex::HostDevice::Atomic::Add(
                            &(phisum(lev, index + 1)), phi_arr(i, j, k));
                        amrex::HostDevice::Atomic::Add(&(ncell(lev, index + 1)),
                                                       1);
                    }
                });

#ifdef AMREX_USE_CUDA
                if (deterministic_nodal_solve) {
                    // turn GPU back on
                    if (launched) Gpu::setLaunchRegion(true);
                }
#endif
            }
        }

        // reduction over boxes to get sum
        ParallelDescriptor::ReduceRealSum(phisum.dataPtr(),
                                          (finest_level + 1) * (nr_irreg + 2));
        ParallelDescriptor::ReduceIntSum(ncell.dataPtr(),
                                         (finest_level + 1) * (nr_irreg + 2));

        // normalize phisum so it actually stores the average at a radius
        for (auto n = 0; n <= finest_level; ++n) {
            for (auto r = 0; r <= nr_irreg; ++r) {
                if (ncell(n, r + 1) > 0) {
                    phisum(n, r + 1) /= Real(ncell(n, r + 1));
                } else {
                    // keep value constant if it is outside the cutoff coords
                    phisum(n, r + 1) = phisum(n, r);
                }
            }
        }

        BaseState<int> which_lev_s(base_geom.nr_fine);
        auto which_lev = which_lev_s.array();
        BaseState<int> max_rcoord_s(fine_lev);
        auto max_rcoord = max_rcoord_s.array();

        // compute center point for the finest level
        phisum(finest_level, 0) = (11.0 / 8.0) * phisum(finest_level, 1) -
                                  (3.0 / 8.0) * phisum(finest_level, 2);
        ncell(finest_level, 0) = 1;

        // choose which level to interpolate from
        const auto dr0 = base_geom.dr(0);
        const auto nrf = base_geom.nr_fine;

        ParallelFor(nrf, [=] AMREX_GPU_DEVICE(int r) {
            Real radius = (Real(r) + 0.5) * dr0;
            // Vector<int> rcoord_p(fine_lev, 0);
            int rcoord_p[MAESTRO_MAX_LEVELS];

            // initialize
            for (int& coord : rcoord_p) {
                coord = 0.0;
            }

            // for each level, find the closest coordinate
            for (auto n = 0; n < fine_lev; ++n) {
                for (auto j = rcoord_p[n]; j <= nr_irreg; ++j) {
                    if (amrex::Math::abs(radius - radii(n, j + 1)) <
                        amrex::Math::abs(radius - radii(n, j + 2))) {
                        rcoord_p[n] = j;
                        break;
                    }
                }
            }

            // make sure closest coordinate is in bounds
            for (auto n = 0; n < fine_lev - 1; ++n) {
                rcoord_p[n] = amrex::max(rcoord_p[n], 1);
            }
            for (auto n = 0; n < fine_lev; ++n) {
                rcoord_p[n] = amrex::min(rcoord_p[n], nr_irreg - 1);
            }

            // choose the level with the largest min over the ncell interpolation points
            which_lev(r) = 0;

            int min_all = amrex::min(ncell(0, rcoord_p[0]),
                                     amrex::min(ncell(0, rcoord_p[0] + 1),
                                                ncell(0, rcoord_p[0] + 2)));

            for (auto n = 1; n < fine_lev; ++n) {
                int min_lev = amrex::min(ncell(n, rcoord_p[n]),
                                         amrex::min(ncell(n, rcoord_p[n] + 1),
                                                    ncell(n, rcoord_p[n] + 2)));

                if (min_lev > min_all) {
                    min_all = min_lev;
                    which_lev(r) = n;
                }
            }

            // if the min hit count at all levels is zero, we expand the search
            // to find the closest instance of where the hitcount becomes nonzero
            int j = 1;
            while (min_all == 0) {
                j++;
                for (auto n = 0; n < fine_lev; ++n) {
                    int min_lev = amrex::max(
                        ncell(n, amrex::max(1, rcoord_p[n] - j) + 1),
                        ncell(n,
                              amrex::min(rcoord_p[n] + j, nr_irreg - 1) + 1));
                    if (min_lev != 0) {
                        which_lev(r) = n;
                        min_all = min_lev;
                        break;
                    }
                }
            }
        });
        Gpu::synchronize();

        // squish the list at each level down to exclude points with no contribution
        for (auto n = 0; n <= finest_level; ++n) {
            int j = 0;
            for (auto r = 0; r <= nr_irreg; ++r) {
                while (ncell(n, j + 1) == 0) {
                    j++;
                    if (j > nr_irreg) {
                        break;
                    }
                }
                if (j > nr_irreg) {
                    for (auto i = r; i <= nr_irreg; ++i) {
                        phisum(n, i + 1) = 1.e99;
                    }
                    for (auto i = r; i <= nr_irreg + 1; ++i) {
                        radii(n, i + 1) = 1.e99;
                    }
                    max_rcoord(n) = r - 1;
                    break;
                }
                phisum(n, r + 1) = phisum(n, j + 1);
                radii(n, r + 1) = radii(n, j + 1);
                ncell(n, r + 1) = ncell(n, j + 1);
                j++;
                if (j > nr_irreg) {
                    max_rcoord(n) = r;
                    break;
                }
            }
        }

        // compute phibar
        const Real drdxfac_loc = drdxfac;
        auto phibar_arr = phibar.array();

        ParallelFor(nrf, [=] AMREX_GPU_DEVICE(int r) {
            Real radius = (Real(r) + 0.5) * dr0;
            int stencil_coord = 0;

            // find the closest coordinate
            for (auto j = stencil_coord; j <= max_rcoord(which_lev(r)); ++j) {
                if (amrex::Math::abs(radius - radii(which_lev(r), j + 1)) <
                    amrex::Math::abs(radius - radii(which_lev(r), j + 2))) {
                    stencil_coord = j;
                    break;
                }
            }

            // make sure the interpolation points will be in bounds
            if (which_lev(r) != fine_lev - 1) {
                stencil_coord = amrex::max(stencil_coord, 1);
            }
            stencil_coord =
                amrex::min(stencil_coord, max_rcoord(which_lev(r)) - 1);

            bool limit =
                (r <= nrf - 1 - drdxfac_loc * pow(2.0, (fine_lev - 2)));

            phibar_arr(0, r) =
                QuadInterp(radius, radii(which_lev(r), stencil_coord),
                           radii(which_lev(r), stencil_coord + 1),
                           radii(which_lev(r), stencil_coord + 2),
                           phisum(which_lev(r), stencil_coord),
                           phisum(which_lev(r), stencil_coord + 1),
                           phisum(which_lev(r), stencil_coord + 2), limit);
        });
        Gpu::synchronize();
    }
}
