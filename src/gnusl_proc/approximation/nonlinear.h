#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <tuple>
#include <sstream>
#include <optional>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>

#include "gnusl_proc/approximation/basic.h"
#include "gnusl_proc/approximation/codes.h"

namespace approximation::nonlinnear
{
    struct data
    {
        size_t n;
        double *t;
        double *y;
    };

    namespace model
    {
        inline int expb_f(const gsl_vector *x, void *data, gsl_vector *f)
        {
            size_t n = ((struct data *)data)->n;
            double *t = ((struct data *)data)->t;
            double *y = ((struct data *)data)->y;

            double A = gsl_vector_get(x, 0);
            double lambda = gsl_vector_get(x, 1);
            double b = gsl_vector_get(x, 2);

            for (size_t i = 0; i < n; i++)
            {
                /* Model Yi = A * exp(-lambda * t_i) + b */
                double Yi = A * exp(-lambda * t[i]) + b;
                gsl_vector_set(f, i, Yi - y[i]);
            }

            return GSL_SUCCESS;
        }

        inline int expb_df(const gsl_vector *x, void *data, gsl_matrix *J)
        {
            size_t n = ((struct data *)data)->n;
            double *t = ((struct data *)data)->t;

            double A = gsl_vector_get(x, 0);
            double lambda = gsl_vector_get(x, 1);

            for (size_t i = 0; i < n; i++)
            {
                /* Jacobian matrix J(i,j) = dfi / dxj, */
                /* where fi = (Yi - yi)/sigma[i],      */
                /*       Yi = A * exp(-lambda * t_i) + b  */
                /* and the xj are the parameters (A,lambda,b) */
                double e = exp(-lambda * t[i]);
                gsl_matrix_set(J, i, 0, e);
                gsl_matrix_set(J, i, 1, -t[i] * A * e);
                gsl_matrix_set(J, i, 2, 1.0);
            }

            return GSL_SUCCESS;
        }
    }

    /// @brief for A * exp(-lambda * t_i) + b
    struct ApproximationResult : public ApproxResultBasic
    {
        ApproximationResult()
        {
            m_method = "nonlinear";
            m_model = "f(x) = A*exp(-lambda*x) + B";
        }

        double A;
        double errA;
        double lambda;
        double errLambda;
        double B;
        double errB;

        double funcInitial;
        double funcFinal;
        double argInitial;
        double argFinal;

        void print()
        {
            std::cout << "A      = " << A << " +/- " << errA << "\n"
                      << "lambda = " << lambda << " +/- " << errLambda << "\n"
                      << "b      = " << B << " +/- " << errB << "\n";
        }
    };

    class ProceedApproximation
    {
    public:
        // setting model
        ProceedApproximation() : p(3)
        {
            /* define the function to be minimized */
            fdf.f = model::expb_f;
            fdf.df = model::expb_df; /* set to NULL for finite-difference Jacobian */

            fdf.fvv = NULL; /* not using geodesic acceleration */

            fdf.p = p;
        }

        int act(const std::vector<double> &dataToFitX, const std::vector<double> &dataToFitY)
        {
            m_nuberPointsToFit = dataToFitY.size();
            m_tMax = dataToFitX.at(m_nuberPointsToFit - 1);

            fdf.n = m_nuberPointsToFit;

            if (dataToFitY.size() < m_nuberPointsToFit)
                return GSL_FAILURE;

            double x_init[3] = {0.0, 0.0, 0.0}; /* starting values */

            gsl_vector_view x = gsl_vector_view_array(x_init, p);

            double weights[m_nuberPointsToFit];
            gsl_vector_view wts = gsl_vector_view_array(weights, m_nuberPointsToFit);

            {
                gsl_rng_env_setup();
                r = gsl_rng_alloc(gsl_rng_default);
            }

            double t[m_nuberPointsToFit],
                y[m_nuberPointsToFit];

            struct data d = {m_nuberPointsToFit, t, y};
            fdf.params = &d;

            /* this is the data to be fitted */
            for (size_t i = 0; i < m_nuberPointsToFit; i++)
            {
                double ti = i * m_tMax / (m_nuberPointsToFit - 1.0);
                double yi = dataToFitY.at(i); // 1.0 + 5 * exp(-1.5 * ti); //
                double si = 0.1 * yi;
                double disturbance = gsl_ran_gaussian(r, si);
                disturbance = 0;

                t[i] = ti;
                y[i] = yi + disturbance;
                weights[i] = 1.0 / (si * si);

                m_resultInformation << "data: " << ti << " " << y[i] << " " << si << "\n";
            }

            const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;

            /* allocate workspace with default parameters */
            gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();
            w = gsl_multifit_nlinear_alloc(T, &fdf_params, m_nuberPointsToFit, p);

            /* initialize solver with starting point and weights */
            gsl_multifit_nlinear_winit(&x.vector, &wts.vector, &fdf, w);

            // copmutition
            proceedWorkspace();

            {
                double dof = m_nuberPointsToFit - p;
                double c = GSL_MAX_DBL(1, sqrt(chisq / dof));

                m_approximationResult.A = gsl_vector_get(w->x, 0);
                m_approximationResult.errA = sqrt(gsl_matrix_get(covar, 0, 0));

                m_approximationResult.lambda = gsl_vector_get(w->x, 1);
                m_approximationResult.errLambda = sqrt(gsl_matrix_get(covar, 1, 1));

                m_approximationResult.B = gsl_vector_get(w->x, 2);
                m_approximationResult.errB = sqrt(gsl_matrix_get(covar, 2, 2));

                m_approximationResult.argInitial = dataToFitX.front();
                m_approximationResult.argFinal = dataToFitX.back();
                m_approximationResult.funcInitial = dataToFitY.front();
                m_approximationResult.funcFinal = dataToFitY.back();
            }

            m_approximationResult.print();

            return GSL_SUCCESS;
        }

        ~ProceedApproximation()
        {
            gsl_multifit_nlinear_free(w);
            gsl_matrix_free(covar);
            gsl_rng_free(r);
        }

        ApproximationResult getApproximationResult() const
        {
            return m_approximationResult;
        }

        std::string getResults()
        {
#define FIT(i) gsl_vector_get(w->x, i)
#define ERR(i) sqrt(gsl_matrix_get(covar, i, i))

            m_resultInformation << "summary from method "
                                << "\'" << gsl_multifit_nlinear_name(w) << "//" << gsl_multifit_nlinear_trs_name(w) << "\'"
                                << "\n"
                                << "number of iterations: "
                                << "\'" << gsl_multifit_nlinear_niter(w) << "\'"
                                << "\n"
                                << "function evaluations: "
                                << "\'" << fdf.nevalf << "\'"
                                << "\n"
                                << "Jacobian evaluations: "
                                << "\'" << fdf.nevaldf << "\'"
                                << "\n"
                                << "reason for stopping: "
                                << "\'" << ((info == 1) ? "small step size" : "small gradient") << "\'"
                                << "\n"
                                << "initial |f(x)| = " << sqrt(chisq0) << "\n"
                                << "final   |f(x)| = " << sqrt(chisq) << "\n";

            double dof = m_nuberPointsToFit - p;
            double c = GSL_MAX_DBL(1, sqrt(chisq / dof));

            m_resultInformation << "chisq/dof = " << chisq / dof << "\n"
                                << "A      = " << FIT(0) << " +/- " << c * ERR(0) << "\n"
                                << "lambda = " << FIT(1) << " +/- " << c * ERR(1) << "\n"
                                << "b      = " << FIT(2) << " +/- " << c * ERR(2) << "\n";

            return m_resultInformation.str();
        }

    private:
        void proceedWorkspace()
        {
            gsl_vector *f;
            /* compute initial cost function */
            f = gsl_multifit_nlinear_residual(w);

            gsl_blas_ddot(f, f, &chisq0);

            const double xtol = 1e-8;
            const double gtol = 1e-8;
            const double ftol = 0.0;
            /* solve the system with a maximum of 100 iterations */
            int status;
            status = gsl_multifit_nlinear_driver(m_nuberPointsToFit,
                                                 xtol, gtol, ftol,
                                                 callbackCalculateJacobian, NULL, &info, w);

            gsl_matrix *J;
            covar = gsl_matrix_alloc(p, p);
            /* compute covariance of best fit parameters */
            J = gsl_multifit_nlinear_jac(w);
            gsl_multifit_nlinear_covar(J, 0.0, covar);

            /* compute final cost */

            gsl_blas_ddot(f, f, &chisq);
        }

        static void callbackCalculateJacobian(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w)
        {
            gsl_vector *f = gsl_multifit_nlinear_residual(w);
            gsl_vector *x = gsl_multifit_nlinear_position(w);
            double rcond;

            /* compute reciprocal condition number of J(x) */
            gsl_multifit_nlinear_rcond(&rcond, w);

            /*m_resultInformation << "iter " << iter << ": "
                                << "A = " << gsl_vector_get(x, 0) << ", "
                                << "lambda = " << gsl_vector_get(x, 1) << ", "
                                << "b = " << gsl_vector_get(x, 2) << ", "
                                << "cond(J) = " << 1.0 / rcond << ", "
                                << "|f(x)| = " << gsl_blas_dnrm2(f) << "\n";*/

            /*fprintf(stderr, "iter %2zu: A = %.4f, lambda = %.4f, b = %.4f, cond(J) = %8.4f, |f(x)| = %.4f\n",
                    iter,
                    gsl_vector_get(x, 0),
                    gsl_vector_get(x, 1),
                    gsl_vector_get(x, 2),
                    1.0 / rcond,
                    gsl_blas_dnrm2(f));*/
        }

        size_t m_nuberPointsToFit;
        double m_tMax;
        gsl_multifit_nlinear_fdf fdf;
        size_t p;
        double chisq0;
        double chisq;

        gsl_multifit_nlinear_workspace *w;
        gsl_matrix *covar;

        int info;
        ApproximationResult m_approximationResult;
        std::stringstream m_resultInformation;

        // extra for testing
        gsl_rng *r;
    };

    /*
    *   Model Yi = A * exp(-lambda * t_i) + b
    */
    std::tuple<int, ApproximationResult>
    approximate(const std::vector<double> &inputDataX,
                const std::vector<double> &inputDataY)
                //const std::optional<std::vector<double>> &err = std::optional<std::vector<double>>())
    {
        approximation::nonlinnear::ProceedApproximation nonlinnear;

        std::ofstream fout("tmp");
        for ( int i = 0; i < inputDataX.size(); i++)
            fout << inputDataX.at(i) << "\t" << inputDataY.at(i) << "\n";
        fout.close();       

        const int errCode = nonlinnear.act(inputDataX, inputDataY);

        if (int(GSL_SUCCESS) == errCode);
        {
            return std::make_tuple(codes::POSITIVE, nonlinnear.getApproximationResult());
        }

        std::cerr << "failed to proceed nonlinnear approximation, errCode: " << errCode << "\n";
        return std::make_tuple(codes::NEGATIVE, nonlinnear.getApproximationResult());
    }

} // namespace
