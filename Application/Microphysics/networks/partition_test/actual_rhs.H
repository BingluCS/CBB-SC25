#ifndef actual_rhs_H
#define actual_rhs_H

#include <AMReX_REAL.H>
#include <AMReX_Array.H>

#include <extern_parameters.H>
#include <actual_network.H>
#include <burn_type.H>
#include <jacobian_utilities.H>
#include <screen.H>
#include <sneut5.H>
#include <reaclib_rates.H>
#include <table_rates.H>
#include <extern_parameters.H>

using namespace amrex;
using namespace ArrayUtil;

using namespace Species;
using namespace Rates;

using namespace rate_tables;


template<class T>
AMREX_GPU_HOST_DEVICE AMREX_INLINE
void ener_gener_rate(T const& dydt, Real& enuc)
{

    // Computes the instantaneous energy generation rate (from the nuclei)

    // This is basically e = m c**2

    enuc = 0.0_rt;

    for (int n = 1; n <= NumSpec; ++n) {
        enuc += dydt(n) * network::mion(n);
    }

    enuc *= C::Legacy::enuc_conv2;
}


template <int do_T_derivatives, typename T>
AMREX_GPU_HOST_DEVICE AMREX_INLINE
void evaluate_rates(const burn_t& state, T& rate_eval) {


    // create molar fractions

    Array1D<Real, 1, NumSpec> Y;
    for (int n = 1; n <= NumSpec; ++n) {
        Y(n) = state.xn[n-1] * aion_inv[n-1];
    }

    [[maybe_unused]] Real rhoy = state.rho * state.y_e;

    // Calculate Reaclib rates

    plasma_state_t pstate;
    fill_plasma_state(pstate, state.T, state.rho, Y);

    tf_t tfactors = evaluate_tfactors(state.T);

    fill_reaclib_rates<do_T_derivatives, T>(tfactors, rate_eval);



    // Evaluate screening factors

    Real ratraw, dratraw_dT;
    Real scor, dscor_dt;
    Real scor2, dscor2_dt;


    {
        constexpr auto scn_fac = scrn::calculate_screen_factor(2.0_rt, 4.0_rt, 26.0_rt, 52.0_rt);


        static_assert(scn_fac.z1 == 2.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_he4_fe52__ni56);
    rate_eval.screened_rates(k_he4_fe52__ni56) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_he4_fe52__ni56);
        rate_eval.dscreened_rates_dT(k_he4_fe52__ni56) = ratraw * dscor_dt + dratraw_dT * scor;
    }

    ratraw = rate_eval.screened_rates(k_he4_fe52__p_co55);
    rate_eval.screened_rates(k_he4_fe52__p_co55) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_he4_fe52__p_co55);
        rate_eval.dscreened_rates_dT(k_he4_fe52__p_co55) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    {
        constexpr auto scn_fac = scrn::calculate_screen_factor(1.0_rt, 1.0_rt, 27.0_rt, 55.0_rt);


        static_assert(scn_fac.z1 == 1.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_p_co55__ni56);
    rate_eval.screened_rates(k_p_co55__ni56) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_p_co55__ni56);
        rate_eval.dscreened_rates_dT(k_p_co55__ni56) = ratraw * dscor_dt + dratraw_dT * scor;
    }

    ratraw = rate_eval.screened_rates(k_p_co55__he4_fe52__derived);
    rate_eval.screened_rates(k_p_co55__he4_fe52__derived) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_p_co55__he4_fe52__derived);
        rate_eval.dscreened_rates_dT(k_p_co55__he4_fe52__derived) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    // Fill approximate rates

    fill_approx_rates<do_T_derivatives, T>(tfactors, rate_eval);

    // Calculate tabular rates

    [[maybe_unused]] Real rate, drate_dt, edot_nu, edot_gamma;


}

AMREX_GPU_HOST_DEVICE AMREX_INLINE
void rhs_nuc(const burn_t& state,
             Array1D<Real, 1, neqs>& ydot_nuc,
             const Array1D<Real, 1, NumSpec>& Y,
             const Array1D<Real, 1, NumRates>& screened_rates) {

    using namespace Rates;

    ydot_nuc(H1) =
        (-screened_rates(k_p_co55__ni56)*Y(Co55)*Y(H1)*state.rho + screened_rates(k_ni56__p_co55__derived)*Y(Ni56)) +
        (screened_rates(k_he4_fe52__p_co55)*Y(Fe52)*Y(He4)*state.rho + -screened_rates(k_p_co55__he4_fe52__derived)*Y(Co55)*Y(H1)*state.rho);

    ydot_nuc(He4) =
        (-screened_rates(k_he4_fe52__ni56)*Y(Fe52)*Y(He4)*state.rho + screened_rates(k_ni56__he4_fe52__derived)*Y(Ni56)) +
        (-screened_rates(k_he4_fe52__p_co55)*Y(Fe52)*Y(He4)*state.rho + screened_rates(k_p_co55__he4_fe52__derived)*Y(Co55)*Y(H1)*state.rho);

    ydot_nuc(Fe52) =
        (-screened_rates(k_he4_fe52__ni56)*Y(Fe52)*Y(He4)*state.rho + screened_rates(k_ni56__he4_fe52__derived)*Y(Ni56)) +
        (-screened_rates(k_he4_fe52__p_co55)*Y(Fe52)*Y(He4)*state.rho + screened_rates(k_p_co55__he4_fe52__derived)*Y(Co55)*Y(H1)*state.rho);

    ydot_nuc(Co55) =
        (-screened_rates(k_p_co55__ni56)*Y(Co55)*Y(H1)*state.rho + screened_rates(k_ni56__p_co55__derived)*Y(Ni56)) +
        (screened_rates(k_he4_fe52__p_co55)*Y(Fe52)*Y(He4)*state.rho + -screened_rates(k_p_co55__he4_fe52__derived)*Y(Co55)*Y(H1)*state.rho);

    ydot_nuc(Ni56) =
        (screened_rates(k_he4_fe52__ni56)*Y(Fe52)*Y(He4)*state.rho + -screened_rates(k_ni56__he4_fe52__derived)*Y(Ni56)) +
        (screened_rates(k_p_co55__ni56)*Y(Co55)*Y(H1)*state.rho + -screened_rates(k_ni56__p_co55__derived)*Y(Ni56));

}


AMREX_GPU_HOST_DEVICE AMREX_INLINE
void actual_rhs (burn_t& state, Array1D<Real, 1, neqs>& ydot)
{
    for (int i = 1; i <= neqs; ++i) {
        ydot(i) = 0.0_rt;
    }


    // Set molar abundances
    Array1D<Real, 1, NumSpec> Y;
    for (int i = 1; i <= NumSpec; ++i) {
        Y(i) = state.xn[i-1] * aion_inv[i-1];
    }

    // build the rates

    rate_t rate_eval;

    constexpr int do_T_derivatives = 0;
    evaluate_rates<do_T_derivatives, rate_t>(state, rate_eval);

    rhs_nuc(state, ydot, Y, rate_eval.screened_rates);

    // ion binding energy contributions

    Real enuc;
    ener_gener_rate(ydot, enuc);

    // include reaction neutrino losses (non-thermal) and gamma heating

    // Get the thermal neutrino losses

    Real sneut, dsneutdt, dsneutdd, snuda, snudz;

    sneut5(state.T, state.rho, state.abar, state.zbar, sneut, dsneutdt, dsneutdd, snuda, snudz);

    // Append the energy equation (this is erg/g/s)

    ydot(net_ienuc) = enuc - sneut;

}


template<class MatrixType>
AMREX_GPU_HOST_DEVICE AMREX_INLINE
void jac_nuc(const burn_t& state,
             MatrixType& jac,
             const Array1D<Real, 1, NumSpec>& Y,
             const Array1D<Real, 1, NumRates>& screened_rates)
{

    Real scratch;

    scratch = -screened_rates(k_p_co55__he4_fe52__derived)*Y(Co55)*state.rho - screened_rates(k_p_co55__ni56)*Y(Co55)*state.rho;
    jac.set(H1, H1, scratch);

    scratch = screened_rates(k_he4_fe52__p_co55)*Y(Fe52)*state.rho;
    jac.set(H1, He4, scratch);

    scratch = screened_rates(k_he4_fe52__p_co55)*Y(He4)*state.rho;
    jac.set(H1, Fe52, scratch);

    scratch = -screened_rates(k_p_co55__he4_fe52__derived)*Y(H1)*state.rho - screened_rates(k_p_co55__ni56)*Y(H1)*state.rho;
    jac.set(H1, Co55, scratch);

    scratch = screened_rates(k_ni56__p_co55__derived);
    jac.set(H1, Ni56, scratch);

    scratch = screened_rates(k_p_co55__he4_fe52__derived)*Y(Co55)*state.rho;
    jac.set(He4, H1, scratch);

    scratch = -screened_rates(k_he4_fe52__ni56)*Y(Fe52)*state.rho - screened_rates(k_he4_fe52__p_co55)*Y(Fe52)*state.rho;
    jac.set(He4, He4, scratch);

    scratch = -screened_rates(k_he4_fe52__ni56)*Y(He4)*state.rho - screened_rates(k_he4_fe52__p_co55)*Y(He4)*state.rho;
    jac.set(He4, Fe52, scratch);

    scratch = screened_rates(k_p_co55__he4_fe52__derived)*Y(H1)*state.rho;
    jac.set(He4, Co55, scratch);

    scratch = screened_rates(k_ni56__he4_fe52__derived);
    jac.set(He4, Ni56, scratch);

    scratch = screened_rates(k_p_co55__he4_fe52__derived)*Y(Co55)*state.rho;
    jac.set(Fe52, H1, scratch);

    scratch = -screened_rates(k_he4_fe52__ni56)*Y(Fe52)*state.rho - screened_rates(k_he4_fe52__p_co55)*Y(Fe52)*state.rho;
    jac.set(Fe52, He4, scratch);

    scratch = -screened_rates(k_he4_fe52__ni56)*Y(He4)*state.rho - screened_rates(k_he4_fe52__p_co55)*Y(He4)*state.rho;
    jac.set(Fe52, Fe52, scratch);

    scratch = screened_rates(k_p_co55__he4_fe52__derived)*Y(H1)*state.rho;
    jac.set(Fe52, Co55, scratch);

    scratch = screened_rates(k_ni56__he4_fe52__derived);
    jac.set(Fe52, Ni56, scratch);

    scratch = -screened_rates(k_p_co55__he4_fe52__derived)*Y(Co55)*state.rho - screened_rates(k_p_co55__ni56)*Y(Co55)*state.rho;
    jac.set(Co55, H1, scratch);

    scratch = screened_rates(k_he4_fe52__p_co55)*Y(Fe52)*state.rho;
    jac.set(Co55, He4, scratch);

    scratch = screened_rates(k_he4_fe52__p_co55)*Y(He4)*state.rho;
    jac.set(Co55, Fe52, scratch);

    scratch = -screened_rates(k_p_co55__he4_fe52__derived)*Y(H1)*state.rho - screened_rates(k_p_co55__ni56)*Y(H1)*state.rho;
    jac.set(Co55, Co55, scratch);

    scratch = screened_rates(k_ni56__p_co55__derived);
    jac.set(Co55, Ni56, scratch);

    scratch = screened_rates(k_p_co55__ni56)*Y(Co55)*state.rho;
    jac.set(Ni56, H1, scratch);

    scratch = screened_rates(k_he4_fe52__ni56)*Y(Fe52)*state.rho;
    jac.set(Ni56, He4, scratch);

    scratch = screened_rates(k_he4_fe52__ni56)*Y(He4)*state.rho;
    jac.set(Ni56, Fe52, scratch);

    scratch = screened_rates(k_p_co55__ni56)*Y(H1)*state.rho;
    jac.set(Ni56, Co55, scratch);

    scratch = -screened_rates(k_ni56__he4_fe52__derived) - screened_rates(k_ni56__p_co55__derived);
    jac.set(Ni56, Ni56, scratch);


}



template<class MatrixType>
AMREX_GPU_HOST_DEVICE AMREX_INLINE
void actual_jac(const burn_t& state, MatrixType& jac)
{

    // Set molar abundances
    Array1D<Real, 1, NumSpec> Y;
    for (int i = 1; i <= NumSpec; ++i) {
        Y(i) = state.xn[i-1] * aion_inv[i-1];
    }


    jac.zero();

    rate_derivs_t rate_eval;

    constexpr int do_T_derivatives = 1;
    evaluate_rates<do_T_derivatives, rate_derivs_t>(state, rate_eval);

    // Species Jacobian elements with respect to other species

    jac_nuc(state, jac, Y, rate_eval.screened_rates);

    // Energy generation rate Jacobian elements with respect to species

    for (int j = 1; j <= NumSpec; ++j) {
        auto jac_slice_2 = [&](int i) -> Real { return jac.get(i, j); };
        ener_gener_rate(jac_slice_2, jac(net_ienuc,j));
    }

    // Account for the thermal neutrino losses

    Real sneut, dsneutdt, dsneutdd, snuda, snudz;
    sneut5(state.T, state.rho, state.abar, state.zbar, sneut, dsneutdt, dsneutdd, snuda, snudz);

    for (int j = 1; j <= NumSpec; ++j) {
       Real b1 = (-state.abar * state.abar * snuda + (zion[j-1] - state.zbar) * state.abar * snudz);
       jac.add(net_ienuc, j, -b1);
    }


    // Evaluate the Jacobian elements with respect to energy by
    // calling the RHS using d(rate) / dT and then transform them
    // to our energy integration variable.

    Array1D<Real, 1, neqs>  yderivs;

    rhs_nuc(state, yderivs, Y, rate_eval.dscreened_rates_dT);

    for (int k = 1; k <= NumSpec; k++) {
        jac.set(k, net_ienuc, temperature_to_energy_jacobian(state, yderivs(k)));
    }


    // finally, d(de/dt)/de

    Real jac_e_T;
    ener_gener_rate(yderivs, jac_e_T);
    jac_e_T -= dsneutdt;
    jac.set(net_ienuc, net_ienuc, temperature_to_energy_jacobian(state, jac_e_T));

}


AMREX_INLINE
void actual_rhs_init () {

    init_tabular();

}


#endif
