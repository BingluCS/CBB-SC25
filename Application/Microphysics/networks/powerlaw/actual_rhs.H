#ifndef ACTUAL_RHS_H
#define ACTUAL_RHS_H

#include <cmath>

#include <AMReX_Array.H>
#include <AMReX_REAL.H>
#include <AMReX_Algorithm.H>

#include <network_properties.H>
#include <extern_parameters.H>
#include <burn_type.H>

using namespace amrex;

void actual_rhs_init();

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void actual_rhs (burn_t& state, Array1D<Real, 1, neqs>& ydot)
{

    for (int n = 1; n <= NumSpec; ++n) {
        ydot(n) = 0.0_rt;
    }

    Real xfueltmp = amrex::max(state.xn[0], 0.0_rt);
    Real dens     = state.rho;
    Real temp     = state.T;

    // Rate is expressed in mass fraction form

    Real rate;

    if (temp < f_act * T_burn_ref) {
        rate = 0.0_rt;
    }
    else {
        rate = rtilde * (dens / rho_burn_ref) * std::pow(xfueltmp, 2) * std::pow(temp / T_burn_ref, nu);
    }

    ydot(1)  = -rate; // fuel
    ydot(2)  =  rate; // ash
    ydot(3) = 0.0_rt; // inert

    // Convert back to molar form

    for (int n = 1; n <= NumSpec; ++n) {
        ydot(n) *= aion_inv[n-1];
    }

    ydot(net_ienuc) = ydot(2) * aion[1] * specific_q_burn;

}


// At present the analytical Jacobian is not implemented.

template<class MatrixType>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void actual_jac (burn_t& state, MatrixType& jac)
{

    for (int j = 1; j <= neqs; ++j) {
        for (int i = 1; i <= neqs; ++i) {
            jac(i,j) = 0.0_rt;
        }
    }

}

#endif
