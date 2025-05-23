#ifndef EOS_COMPOSITION_H
#define EOS_COMPOSITION_H

#include <AMReX_BLFort.H>
#include <network.H>
#include <eos_type.H>

using namespace amrex;

struct eos_xderivs_t {
  Real dedX[NumSpec];
  Real dpdX[NumSpec];
  Real dhdX[NumSpec];
};

// Given a set of mass fractions, calculate quantities that depend
// on the composition like abar and zbar.

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_INLINE
void subroutine (T& state)
{
    // Calculate abar, the mean nucleon number,
    // zbar, the mean proton number,
    // mu, the mean molecular weight,
    // mu_e, the mean number of nucleons per electron, and
    // y_e, the electron fraction.

    state.y_e = 0.0_rt;
    for (int n = 0; n < NumSpec; ++n) {
        state.y_e = state.y_e + state.xn[n] * zion[n] * aion_inv[n];
    }
    state.mu_e = 1.0_rt / state.y_e;

    Real sum = 0.0_rt;
    for (int n = 0; n < NumSpec; ++n) {
        sum = sum + state.xn[n] * aion_inv[n];
    }
    state.abar = 1.0_rt / sum;
    state.zbar = state.abar / state.mu_e;
}



// Compute thermodynamic derivatives with respect to xn(:)

#ifdef EXTRA_THERMO
template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_INLINE
eos_x_derivs_t composition_derivatives (const T& state)
{
    eos_xderivs_t state_xderivs;

    // Get the mass of a nucleon from Avogadro's number.
    const Real m_nucleon = 1.0_rt / n_A;

    // Composition derivatives

    for (int n = 0; n < NumSpec; ++n) {
        state_xderivs.dpdX[n] = state.rho * (k_B / m_nucleon) * state.T * aion_inv[n];
        state_xderivs.dedX[n] = (k_B / m_nucleon) * state.T * aion_inv[n] / (gammas[n] - 1.0_rt);
        state_xderivs.dhdX[n] = state_xderivs.dedX[n] + (state.p / (state.rho * state.rho) - state.dedr) *
                                                        state_xderivs.dpdX[n] / state.dpdr;
    }

    return state_xderivs;
}
#endif

#endif
