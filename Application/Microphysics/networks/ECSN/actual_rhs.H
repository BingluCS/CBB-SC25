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
        constexpr auto scn_fac = scrn::calculate_screen_factor(2.0_rt, 4.0_rt, 8.0_rt, 16.0_rt);


        static_assert(scn_fac.z1 == 2.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_he4_o16__ne20);
    rate_eval.screened_rates(k_he4_o16__ne20) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_he4_o16__ne20);
        rate_eval.dscreened_rates_dT(k_he4_o16__ne20) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    {
        constexpr auto scn_fac = scrn::calculate_screen_factor(2.0_rt, 4.0_rt, 10.0_rt, 20.0_rt);


        static_assert(scn_fac.z1 == 2.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_he4_ne20__mg24);
    rate_eval.screened_rates(k_he4_ne20__mg24) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_he4_ne20__mg24);
        rate_eval.dscreened_rates_dT(k_he4_ne20__mg24) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    {
        constexpr auto scn_fac = scrn::calculate_screen_factor(2.0_rt, 4.0_rt, 12.0_rt, 24.0_rt);


        static_assert(scn_fac.z1 == 2.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_he4_mg24__si28);
    rate_eval.screened_rates(k_he4_mg24__si28) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_he4_mg24__si28);
        rate_eval.dscreened_rates_dT(k_he4_mg24__si28) = ratraw * dscor_dt + dratraw_dT * scor;
    }

    ratraw = rate_eval.screened_rates(k_he4_mg24__p_al27);
    rate_eval.screened_rates(k_he4_mg24__p_al27) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_he4_mg24__p_al27);
        rate_eval.dscreened_rates_dT(k_he4_mg24__p_al27) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    {
        constexpr auto scn_fac = scrn::calculate_screen_factor(1.0_rt, 1.0_rt, 13.0_rt, 27.0_rt);


        static_assert(scn_fac.z1 == 1.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_p_al27__si28);
    rate_eval.screened_rates(k_p_al27__si28) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_p_al27__si28);
        rate_eval.dscreened_rates_dT(k_p_al27__si28) = ratraw * dscor_dt + dratraw_dT * scor;
    }

    ratraw = rate_eval.screened_rates(k_p_al27__he4_mg24);
    rate_eval.screened_rates(k_p_al27__he4_mg24) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_p_al27__he4_mg24);
        rate_eval.dscreened_rates_dT(k_p_al27__he4_mg24) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    {
        constexpr auto scn_fac = scrn::calculate_screen_factor(2.0_rt, 4.0_rt, 13.0_rt, 27.0_rt);


        static_assert(scn_fac.z1 == 2.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_he4_al27__p31);
    rate_eval.screened_rates(k_he4_al27__p31) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_he4_al27__p31);
        rate_eval.dscreened_rates_dT(k_he4_al27__p31) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    {
        constexpr auto scn_fac = scrn::calculate_screen_factor(2.0_rt, 4.0_rt, 14.0_rt, 28.0_rt);


        static_assert(scn_fac.z1 == 2.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_he4_si28__s32);
    rate_eval.screened_rates(k_he4_si28__s32) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_he4_si28__s32);
        rate_eval.dscreened_rates_dT(k_he4_si28__s32) = ratraw * dscor_dt + dratraw_dT * scor;
    }

    ratraw = rate_eval.screened_rates(k_he4_si28__p_p31);
    rate_eval.screened_rates(k_he4_si28__p_p31) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_he4_si28__p_p31);
        rate_eval.dscreened_rates_dT(k_he4_si28__p_p31) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    {
        constexpr auto scn_fac = scrn::calculate_screen_factor(1.0_rt, 1.0_rt, 15.0_rt, 31.0_rt);


        static_assert(scn_fac.z1 == 1.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_p_p31__s32);
    rate_eval.screened_rates(k_p_p31__s32) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_p_p31__s32);
        rate_eval.dscreened_rates_dT(k_p_p31__s32) = ratraw * dscor_dt + dratraw_dT * scor;
    }

    ratraw = rate_eval.screened_rates(k_p_p31__he4_si28);
    rate_eval.screened_rates(k_p_p31__he4_si28) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_p_p31__he4_si28);
        rate_eval.dscreened_rates_dT(k_p_p31__he4_si28) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    {
        constexpr auto scn_fac = scrn::calculate_screen_factor(8.0_rt, 16.0_rt, 8.0_rt, 16.0_rt);


        static_assert(scn_fac.z1 == 8.0_rt);


        actual_screen<do_T_derivatives>(pstate, scn_fac, scor, dscor_dt);
    }


    ratraw = rate_eval.screened_rates(k_o16_o16__p_p31);
    rate_eval.screened_rates(k_o16_o16__p_p31) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_o16_o16__p_p31);
        rate_eval.dscreened_rates_dT(k_o16_o16__p_p31) = ratraw * dscor_dt + dratraw_dT * scor;
    }

    ratraw = rate_eval.screened_rates(k_o16_o16__he4_si28);
    rate_eval.screened_rates(k_o16_o16__he4_si28) *= scor;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        dratraw_dT = rate_eval.dscreened_rates_dT(k_o16_o16__he4_si28);
        rate_eval.dscreened_rates_dT(k_o16_o16__he4_si28) = ratraw * dscor_dt + dratraw_dT * scor;
    }


    // Fill approximate rates

    fill_approx_rates<do_T_derivatives, T>(tfactors, rate_eval);

    // Calculate tabular rates

    [[maybe_unused]] Real rate, drate_dt, edot_nu, edot_gamma;

    tabular_evaluate(j_f20_o20_meta, j_f20_o20_rhoy, j_f20_o20_temp, j_f20_o20_data,
                     rhoy, state.T, rate, drate_dt, edot_nu, edot_gamma);
    rate_eval.screened_rates(k_f20__o20) = rate;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        rate_eval.dscreened_rates_dT(k_f20__o20) = drate_dt;
    }
    rate_eval.add_energy_rate(k_f20__o20) = edot_nu + edot_gamma;

    tabular_evaluate(j_ne20_f20_meta, j_ne20_f20_rhoy, j_ne20_f20_temp, j_ne20_f20_data,
                     rhoy, state.T, rate, drate_dt, edot_nu, edot_gamma);
    rate_eval.screened_rates(k_ne20__f20) = rate;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        rate_eval.dscreened_rates_dT(k_ne20__f20) = drate_dt;
    }
    rate_eval.add_energy_rate(k_ne20__f20) = edot_nu + edot_gamma;

    tabular_evaluate(j_o20_f20_meta, j_o20_f20_rhoy, j_o20_f20_temp, j_o20_f20_data,
                     rhoy, state.T, rate, drate_dt, edot_nu, edot_gamma);
    rate_eval.screened_rates(k_o20__f20) = rate;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        rate_eval.dscreened_rates_dT(k_o20__f20) = drate_dt;
    }
    rate_eval.add_energy_rate(k_o20__f20) = edot_nu + edot_gamma;

    tabular_evaluate(j_f20_ne20_meta, j_f20_ne20_rhoy, j_f20_ne20_temp, j_f20_ne20_data,
                     rhoy, state.T, rate, drate_dt, edot_nu, edot_gamma);
    rate_eval.screened_rates(k_f20__ne20) = rate;
    if constexpr (std::is_same<T, rate_derivs_t>::value) {
        rate_eval.dscreened_rates_dT(k_f20__ne20) = drate_dt;
    }
    rate_eval.add_energy_rate(k_f20__ne20) = edot_nu + edot_gamma;


}

AMREX_GPU_HOST_DEVICE AMREX_INLINE
void rhs_nuc(const burn_t& state,
             Array1D<Real, 1, neqs>& ydot_nuc,
             const Array1D<Real, 1, NumSpec>& Y,
             const Array1D<Real, 1, NumRates>& screened_rates) {

    using namespace Rates;

    ydot_nuc(H1) =
        -screened_rates(k_p_al27__si28)*Y(Al27)*Y(H1)*state.rho +
        -screened_rates(k_p_p31__s32)*Y(P31)*Y(H1)*state.rho +
        0.5*screened_rates(k_o16_o16__p_p31)*std::pow(Y(O16), 2)*state.rho +
        (-screened_rates(k_p_al27__he4_mg24)*Y(Al27)*Y(H1)*state.rho + screened_rates(k_he4_mg24__p_al27)*Y(He4)*Y(Mg24)*state.rho) +
        (-screened_rates(k_p_p31__he4_si28)*Y(P31)*Y(H1)*state.rho + screened_rates(k_he4_si28__p_p31)*Y(He4)*Y(Si28)*state.rho);

    ydot_nuc(He4) =
        (-screened_rates(k_he4_o16__ne20)*Y(He4)*Y(O16)*state.rho + screened_rates(k_ne20__he4_o16)*Y(Ne20)) +
        -screened_rates(k_he4_ne20__mg24)*Y(He4)*Y(Ne20)*state.rho +
        -screened_rates(k_he4_mg24__si28)*Y(He4)*Y(Mg24)*state.rho +
        -screened_rates(k_he4_al27__p31)*Y(Al27)*Y(He4)*state.rho +
        -screened_rates(k_he4_si28__s32)*Y(He4)*Y(Si28)*state.rho +
        0.5*screened_rates(k_o16_o16__he4_si28)*std::pow(Y(O16), 2)*state.rho +
        (screened_rates(k_p_al27__he4_mg24)*Y(Al27)*Y(H1)*state.rho + -screened_rates(k_he4_mg24__p_al27)*Y(He4)*Y(Mg24)*state.rho) +
        (screened_rates(k_p_p31__he4_si28)*Y(P31)*Y(H1)*state.rho + -screened_rates(k_he4_si28__p_p31)*Y(He4)*Y(Si28)*state.rho);

    ydot_nuc(O16) =
        (-screened_rates(k_he4_o16__ne20)*Y(He4)*Y(O16)*state.rho + screened_rates(k_ne20__he4_o16)*Y(Ne20)) +
        -screened_rates(k_o16_o16__p_p31)*std::pow(Y(O16), 2)*state.rho +
        -screened_rates(k_o16_o16__he4_si28)*std::pow(Y(O16), 2)*state.rho;

    ydot_nuc(O20) =
        (-screened_rates(k_o20__f20)*Y(O20) + screened_rates(k_f20__o20)*Y(F20));

    ydot_nuc(F20) =
        (screened_rates(k_o20__f20)*Y(O20) + -screened_rates(k_f20__o20)*Y(F20)) +
        (-screened_rates(k_f20__ne20)*Y(F20) + screened_rates(k_ne20__f20)*Y(Ne20));

    ydot_nuc(Ne20) =
        (screened_rates(k_he4_o16__ne20)*Y(He4)*Y(O16)*state.rho + -screened_rates(k_ne20__he4_o16)*Y(Ne20)) +
        -screened_rates(k_he4_ne20__mg24)*Y(He4)*Y(Ne20)*state.rho +
        (screened_rates(k_f20__ne20)*Y(F20) + -screened_rates(k_ne20__f20)*Y(Ne20));

    ydot_nuc(Mg24) =
        screened_rates(k_he4_ne20__mg24)*Y(He4)*Y(Ne20)*state.rho +
        -screened_rates(k_he4_mg24__si28)*Y(He4)*Y(Mg24)*state.rho +
        (screened_rates(k_p_al27__he4_mg24)*Y(Al27)*Y(H1)*state.rho + -screened_rates(k_he4_mg24__p_al27)*Y(He4)*Y(Mg24)*state.rho);

    ydot_nuc(Al27) =
        -screened_rates(k_p_al27__si28)*Y(Al27)*Y(H1)*state.rho +
        -screened_rates(k_he4_al27__p31)*Y(Al27)*Y(He4)*state.rho +
        (-screened_rates(k_p_al27__he4_mg24)*Y(Al27)*Y(H1)*state.rho + screened_rates(k_he4_mg24__p_al27)*Y(He4)*Y(Mg24)*state.rho);

    ydot_nuc(Si28) =
        screened_rates(k_he4_mg24__si28)*Y(He4)*Y(Mg24)*state.rho +
        screened_rates(k_p_al27__si28)*Y(Al27)*Y(H1)*state.rho +
        -screened_rates(k_he4_si28__s32)*Y(He4)*Y(Si28)*state.rho +
        0.5*screened_rates(k_o16_o16__he4_si28)*std::pow(Y(O16), 2)*state.rho +
        (screened_rates(k_p_p31__he4_si28)*Y(P31)*Y(H1)*state.rho + -screened_rates(k_he4_si28__p_p31)*Y(He4)*Y(Si28)*state.rho);

    ydot_nuc(P31) =
        screened_rates(k_he4_al27__p31)*Y(Al27)*Y(He4)*state.rho +
        -screened_rates(k_p_p31__s32)*Y(P31)*Y(H1)*state.rho +
        0.5*screened_rates(k_o16_o16__p_p31)*std::pow(Y(O16), 2)*state.rho +
        (-screened_rates(k_p_p31__he4_si28)*Y(P31)*Y(H1)*state.rho + screened_rates(k_he4_si28__p_p31)*Y(He4)*Y(Si28)*state.rho);

    ydot_nuc(S32) =
        screened_rates(k_he4_si28__s32)*Y(He4)*Y(Si28)*state.rho +
        screened_rates(k_p_p31__s32)*Y(P31)*Y(H1)*state.rho;

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
    enuc += C::Legacy::n_A * Y(F20) * rate_eval.add_energy_rate(k_f20__o20);
    enuc += C::Legacy::n_A * Y(Ne20) * rate_eval.add_energy_rate(k_ne20__f20);
    enuc += C::Legacy::n_A * Y(O20) * rate_eval.add_energy_rate(k_o20__f20);
    enuc += C::Legacy::n_A * Y(F20) * rate_eval.add_energy_rate(k_f20__ne20);

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

    scratch = -screened_rates(k_p_al27__he4_mg24)*Y(Al27)*state.rho - screened_rates(k_p_al27__si28)*Y(Al27)*state.rho - screened_rates(k_p_p31__he4_si28)*Y(P31)*state.rho - screened_rates(k_p_p31__s32)*Y(P31)*state.rho;
    jac.set(H1, H1, scratch);

    scratch = screened_rates(k_he4_mg24__p_al27)*Y(Mg24)*state.rho + screened_rates(k_he4_si28__p_p31)*Y(Si28)*state.rho;
    jac.set(H1, He4, scratch);

    scratch = 1.0*screened_rates(k_o16_o16__p_p31)*Y(O16)*state.rho;
    jac.set(H1, O16, scratch);

    scratch = screened_rates(k_he4_mg24__p_al27)*Y(He4)*state.rho;
    jac.set(H1, Mg24, scratch);

    scratch = -screened_rates(k_p_al27__he4_mg24)*Y(H1)*state.rho - screened_rates(k_p_al27__si28)*Y(H1)*state.rho;
    jac.set(H1, Al27, scratch);

    scratch = screened_rates(k_he4_si28__p_p31)*Y(He4)*state.rho;
    jac.set(H1, Si28, scratch);

    scratch = -screened_rates(k_p_p31__he4_si28)*Y(H1)*state.rho - screened_rates(k_p_p31__s32)*Y(H1)*state.rho;
    jac.set(H1, P31, scratch);

    scratch = screened_rates(k_p_al27__he4_mg24)*Y(Al27)*state.rho + screened_rates(k_p_p31__he4_si28)*Y(P31)*state.rho;
    jac.set(He4, H1, scratch);

    scratch = -screened_rates(k_he4_al27__p31)*Y(Al27)*state.rho - screened_rates(k_he4_mg24__p_al27)*Y(Mg24)*state.rho - screened_rates(k_he4_mg24__si28)*Y(Mg24)*state.rho - screened_rates(k_he4_ne20__mg24)*Y(Ne20)*state.rho - screened_rates(k_he4_o16__ne20)*Y(O16)*state.rho - screened_rates(k_he4_si28__p_p31)*Y(Si28)*state.rho - screened_rates(k_he4_si28__s32)*Y(Si28)*state.rho;
    jac.set(He4, He4, scratch);

    scratch = -screened_rates(k_he4_o16__ne20)*Y(He4)*state.rho + 1.0*screened_rates(k_o16_o16__he4_si28)*Y(O16)*state.rho;
    jac.set(He4, O16, scratch);

    scratch = -screened_rates(k_he4_ne20__mg24)*Y(He4)*state.rho + screened_rates(k_ne20__he4_o16);
    jac.set(He4, Ne20, scratch);

    scratch = -screened_rates(k_he4_mg24__p_al27)*Y(He4)*state.rho - screened_rates(k_he4_mg24__si28)*Y(He4)*state.rho;
    jac.set(He4, Mg24, scratch);

    scratch = -screened_rates(k_he4_al27__p31)*Y(He4)*state.rho + screened_rates(k_p_al27__he4_mg24)*Y(H1)*state.rho;
    jac.set(He4, Al27, scratch);

    scratch = -screened_rates(k_he4_si28__p_p31)*Y(He4)*state.rho - screened_rates(k_he4_si28__s32)*Y(He4)*state.rho;
    jac.set(He4, Si28, scratch);

    scratch = screened_rates(k_p_p31__he4_si28)*Y(H1)*state.rho;
    jac.set(He4, P31, scratch);

    scratch = -screened_rates(k_he4_o16__ne20)*Y(O16)*state.rho;
    jac.set(O16, He4, scratch);

    scratch = -screened_rates(k_he4_o16__ne20)*Y(He4)*state.rho - 2.0*screened_rates(k_o16_o16__he4_si28)*Y(O16)*state.rho - 2.0*screened_rates(k_o16_o16__p_p31)*Y(O16)*state.rho;
    jac.set(O16, O16, scratch);

    scratch = screened_rates(k_ne20__he4_o16);
    jac.set(O16, Ne20, scratch);

    scratch = -screened_rates(k_o20__f20);
    jac.set(O20, O20, scratch);

    scratch = screened_rates(k_f20__o20);
    jac.set(O20, F20, scratch);

    scratch = screened_rates(k_o20__f20);
    jac.set(F20, O20, scratch);

    scratch = -screened_rates(k_f20__ne20) - screened_rates(k_f20__o20);
    jac.set(F20, F20, scratch);

    scratch = screened_rates(k_ne20__f20);
    jac.set(F20, Ne20, scratch);

    scratch = -screened_rates(k_he4_ne20__mg24)*Y(Ne20)*state.rho + screened_rates(k_he4_o16__ne20)*Y(O16)*state.rho;
    jac.set(Ne20, He4, scratch);

    scratch = screened_rates(k_he4_o16__ne20)*Y(He4)*state.rho;
    jac.set(Ne20, O16, scratch);

    scratch = screened_rates(k_f20__ne20);
    jac.set(Ne20, F20, scratch);

    scratch = -screened_rates(k_he4_ne20__mg24)*Y(He4)*state.rho - screened_rates(k_ne20__f20) - screened_rates(k_ne20__he4_o16);
    jac.set(Ne20, Ne20, scratch);

    scratch = screened_rates(k_p_al27__he4_mg24)*Y(Al27)*state.rho;
    jac.set(Mg24, H1, scratch);

    scratch = -screened_rates(k_he4_mg24__p_al27)*Y(Mg24)*state.rho - screened_rates(k_he4_mg24__si28)*Y(Mg24)*state.rho + screened_rates(k_he4_ne20__mg24)*Y(Ne20)*state.rho;
    jac.set(Mg24, He4, scratch);

    scratch = screened_rates(k_he4_ne20__mg24)*Y(He4)*state.rho;
    jac.set(Mg24, Ne20, scratch);

    scratch = -screened_rates(k_he4_mg24__p_al27)*Y(He4)*state.rho - screened_rates(k_he4_mg24__si28)*Y(He4)*state.rho;
    jac.set(Mg24, Mg24, scratch);

    scratch = screened_rates(k_p_al27__he4_mg24)*Y(H1)*state.rho;
    jac.set(Mg24, Al27, scratch);

    scratch = -screened_rates(k_p_al27__he4_mg24)*Y(Al27)*state.rho - screened_rates(k_p_al27__si28)*Y(Al27)*state.rho;
    jac.set(Al27, H1, scratch);

    scratch = -screened_rates(k_he4_al27__p31)*Y(Al27)*state.rho + screened_rates(k_he4_mg24__p_al27)*Y(Mg24)*state.rho;
    jac.set(Al27, He4, scratch);

    scratch = screened_rates(k_he4_mg24__p_al27)*Y(He4)*state.rho;
    jac.set(Al27, Mg24, scratch);

    scratch = -screened_rates(k_he4_al27__p31)*Y(He4)*state.rho - screened_rates(k_p_al27__he4_mg24)*Y(H1)*state.rho - screened_rates(k_p_al27__si28)*Y(H1)*state.rho;
    jac.set(Al27, Al27, scratch);

    scratch = screened_rates(k_p_al27__si28)*Y(Al27)*state.rho + screened_rates(k_p_p31__he4_si28)*Y(P31)*state.rho;
    jac.set(Si28, H1, scratch);

    scratch = screened_rates(k_he4_mg24__si28)*Y(Mg24)*state.rho - screened_rates(k_he4_si28__p_p31)*Y(Si28)*state.rho - screened_rates(k_he4_si28__s32)*Y(Si28)*state.rho;
    jac.set(Si28, He4, scratch);

    scratch = 1.0*screened_rates(k_o16_o16__he4_si28)*Y(O16)*state.rho;
    jac.set(Si28, O16, scratch);

    scratch = screened_rates(k_he4_mg24__si28)*Y(He4)*state.rho;
    jac.set(Si28, Mg24, scratch);

    scratch = screened_rates(k_p_al27__si28)*Y(H1)*state.rho;
    jac.set(Si28, Al27, scratch);

    scratch = -screened_rates(k_he4_si28__p_p31)*Y(He4)*state.rho - screened_rates(k_he4_si28__s32)*Y(He4)*state.rho;
    jac.set(Si28, Si28, scratch);

    scratch = screened_rates(k_p_p31__he4_si28)*Y(H1)*state.rho;
    jac.set(Si28, P31, scratch);

    scratch = -screened_rates(k_p_p31__he4_si28)*Y(P31)*state.rho - screened_rates(k_p_p31__s32)*Y(P31)*state.rho;
    jac.set(P31, H1, scratch);

    scratch = screened_rates(k_he4_al27__p31)*Y(Al27)*state.rho + screened_rates(k_he4_si28__p_p31)*Y(Si28)*state.rho;
    jac.set(P31, He4, scratch);

    scratch = 1.0*screened_rates(k_o16_o16__p_p31)*Y(O16)*state.rho;
    jac.set(P31, O16, scratch);

    scratch = screened_rates(k_he4_al27__p31)*Y(He4)*state.rho;
    jac.set(P31, Al27, scratch);

    scratch = screened_rates(k_he4_si28__p_p31)*Y(He4)*state.rho;
    jac.set(P31, Si28, scratch);

    scratch = -screened_rates(k_p_p31__he4_si28)*Y(H1)*state.rho - screened_rates(k_p_p31__s32)*Y(H1)*state.rho;
    jac.set(P31, P31, scratch);

    scratch = screened_rates(k_p_p31__s32)*Y(P31)*state.rho;
    jac.set(S32, H1, scratch);

    scratch = screened_rates(k_he4_si28__s32)*Y(Si28)*state.rho;
    jac.set(S32, He4, scratch);

    scratch = screened_rates(k_he4_si28__s32)*Y(He4)*state.rho;
    jac.set(S32, Si28, scratch);

    scratch = screened_rates(k_p_p31__s32)*Y(H1)*state.rho;
    jac.set(S32, P31, scratch);


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
