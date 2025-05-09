#ifndef VODE_DVNLSD_H
#define VODE_DVNLSD_H

#include <vode_type.H>
#ifndef NEW_NETWORK_IMPLEMENTATION
#include <linpack.H>
#endif
#include <vode_dvjac.H>

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
Real dvnlsd (IArray1D& pivot, int& NFLAG, burn_t& state, dvode_t& vstate)
{

    Real ACNRM = 1.e10_rt;

    // dvnlsd is a nonlinear system solver that uses a chord (modified
    // Newton) method with direct linear algebraic system solvers.
    // It then handles the corrector phase of this integration package.

    // Parameter declarations
    const Real CCMAX = 0.3e0_rt;
    const Real CRDOWN = 0.3e0_rt;
    const Real RDIV  = 2.0e0_rt;
    const int MAXCOR = 3;
    const int MSBP = 20;

    Real DEL;
    int M;

    // On the first step, on a change of method order, or after a
    // nonlinear convergence failure with NFLAG = -2, set IPUP = 1
    // to force a Jacobian update.

    if (NFLAG == 0) {
        vstate.ICF = 0;
    }
    if (NFLAG == -2) {
        vstate.IPUP = 1;
    }

    // RC is the ratio of new to old values of the coefficient H / EL(2) = h / l1.
    // When RC differs from 1 by more than CCMAX, IPUP is set to 1
    // to force DVJAC to be called, if a Jacobian is involved.
    // In any case, DVJAC is called at least every MSBP steps.

    vstate.DRC = std::abs(vstate.RC - 1.0_rt);
    if (vstate.DRC > CCMAX || vstate.NST >= vstate.NSLP + MSBP) {
        vstate.IPUP = 1;
    }

    // Up to MAXCOR corrector iterations are taken.  A convergence test is
    // made on the r.m.s. norm of each correction, weighted by the error
    // weight array ewt.  The sum of the corrections is accumulated in the
    // array acor.  The YH array is not altered in the corrector loop.

    bool converged = false;

    while (true) {

        M = 0;
        Real DELP = 0.0_rt;

        for (int i = 1; i <= VODE_NEQS; ++i) {
            vstate.y(i) = vstate.yh(i,1);
        }

        rhs(vstate.tn, state, vstate, vstate.savf);
        vstate.NFE += 1;

        if (vstate.IPUP == 1) {

            // If indicated, the matrix P = I - h*rl1*J is reevaluated and
            // preprocessed before starting the corrector iteration.  IPUP is set
            // to 0 as an indicator that this has been done.

            int IERPJ;
            dvjac(pivot, IERPJ, state, vstate);
            vstate.IPUP = 0;
            vstate.RC = 1.0_rt;
            vstate.DRC = 0.0_rt;
            vstate.CRATE = 1.0_rt;
            vstate.NSLP = vstate.NST;

            // If matrix is singular, take error return to force cut in step size.
            if (IERPJ != 0) {
                NFLAG = -1;
                vstate.ICF = 2;
                vstate.IPUP = 1;
                return ACNRM;
            }

        }

        for (int i = 1; i <= VODE_NEQS; ++i) {
            vstate.acor(i) = 0.0_rt;
        }

        // Corrector iteration loop.

        while (true) {

            // Compute the corrector error, and solve the linear system with
            // that as right-hand side and  P as coefficient matrix. The
            // correction is scaled by the factor 2/(1+RC) to account for
            // changes in h*rl1 since the last dvjac call.

            for (int i = 1; i <= VODE_NEQS; ++i) {
                vstate.y(i) = (vstate.RL1 * vstate.H) * vstate.savf(i) -
                              (vstate.RL1 * vstate.yh(i,2) + vstate.acor(i));
            }

#ifdef NEW_NETWORK_IMPLEMENTATION
            RHS::dgesl(vstate.jac, vstate.y);
#else
            dgesl<VODE_NEQS>(vstate.jac, pivot, vstate.y);
#endif

            if (vstate.RC != 1.0_rt) {
                Real CSCALE = 2.0_rt / (1.0_rt + vstate.RC);
                for (int i = 1; i <= VODE_NEQS; ++i) {
                    vstate.y(i) *= CSCALE;
                }
            }

            DEL = 0.0_rt;
            for (int i = 1; i <= VODE_NEQS; ++i) {
                DEL += (vstate.y(i) * vstate.ewt(i)) * (vstate.y(i) * vstate.ewt(i));
            }
            DEL = std::sqrt(DEL / VODE_NEQS);

            for (int i = 1; i <= VODE_NEQS; ++i) {
                vstate.acor(i) += vstate.y(i);
            }

            for (int i = 1; i <= VODE_NEQS; ++i) {
                vstate.y(i) = vstate.yh(i,1) + vstate.acor(i);
            }

            // Test for convergence. If M > 0, an estimate of the convergence
            // rate constant is stored in CRATE, and this is used in the test.

            if (M != 0) {
                vstate.CRATE = amrex::max(CRDOWN * vstate.CRATE, DEL / DELP);
            }

            Real DCON = DEL * amrex::min(1.0_rt, vstate.CRATE) / vstate.tq(4);
            if (DCON <= 1.0_rt) {
                // we converged, exit the outer loop
                converged = true;
                break;
            }

            M += 1;
            if (M == MAXCOR) {
                //! exit the inner correction iteration
                break;
            }

            if (M >= 2 && DEL > RDIV * DELP) {
                // exit the inner correction iteration
                break;
            }

            DELP = DEL;
            rhs(vstate.tn, state, vstate, vstate.savf);
            vstate.NFE += 1;

        }

        if (converged) {
            break;
        }

        if (vstate.JCUR == 1) {
            NFLAG = -1;
            vstate.ICF = 2;
            vstate.IPUP = 1;
            return ACNRM;
        }

        vstate.ICF = 1;
        vstate.IPUP = 1;

    }

    // Return for successful step.
    NFLAG = 0;
    vstate.JCUR = 0;
    vstate.ICF = 0;
    if (M == 0) {
        ACNRM = DEL;
    }

    if (M > 0) {
        ACNRM = 0.0_rt;
        for (int i = 1; i <= VODE_NEQS; ++i) {
            ACNRM += (vstate.acor(i) * vstate.ewt(i)) * (vstate.acor(i) * vstate.ewt(i));
        }
        ACNRM = std::sqrt(ACNRM / VODE_NEQS);
    }

    return ACNRM;

}

#endif
