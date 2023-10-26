#pragma once

#include <stdio.h>

#include <vector>
#include <tuple>

#include <gsl/gsl_multifit.h>

#include "codes.h"

// https://www.gnu.org/software/gsl/doc/html/lls.html?highlight=approx#c.gsl_multifit_wlinear
std::tuple<int, std::vector<double>>
approxiamteQadratic(const std::vector<double>& inputX,
                    const std::vector<double>& inputY,
                    std::vector<double> err = std::vector<double>())
{
    if (inputX.size() != inputY.size())
        return std::make_tuple(approximation::codes::NEGATIVE, std::vector<double>());

    if (inputX.size() < 3)
    {
        return std::make_tuple(approximation::codes::NEGATIVE, std::vector<double>());
    }

    if (err.empty())
        err.assign(inputX.size(), 0.001);

    const int inputDataSize = inputX.size();

    double chisq;
    gsl_matrix *X, *cov;
    gsl_vector *y, *w, *c;

    X = gsl_matrix_alloc(inputDataSize, 3);
    y = gsl_vector_alloc(inputDataSize);
    w = gsl_vector_alloc(inputDataSize);

    c = gsl_vector_alloc(3);
    cov = gsl_matrix_alloc(3, 3);

    for (int i = 0; i < inputDataSize; i++)
    {
        gsl_matrix_set(X, i, 0, 1.0);
        gsl_matrix_set(X, i, 1, inputX[i]);
        gsl_matrix_set(X, i, 2, inputX[i] * inputX[i]);

        gsl_vector_set(y, i, inputY[i]);
        gsl_vector_set(w, i, 1.0 / (err[i]*err[i]));
    }

    {
        gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(inputDataSize, 3);
        gsl_multifit_wlinear(X, w, y, c, cov,
                             &chisq, work);
        gsl_multifit_linear_free(work);
    }
/*
#define C(i) (gsl_vector_get(c, (i)))
#define COV(i, j) (gsl_matrix_get(cov, (i), (j)))

    {
        printf("# best fit: Y = %g + %g X + %g X^2\n",
               C(0), C(1), C(2));

        printf("# covariance matrix:\n");
        printf("[ %+.5e, %+.5e, %+.5e  \n",
               COV(0, 0), COV(0, 1), COV(0, 2));
        printf("  %+.5e, %+.5e, %+.5e  \n",
               COV(1, 0), COV(1, 1), COV(1, 2));
        printf("  %+.5e, %+.5e, %+.5e ]\n",
               COV(2, 0), COV(2, 1), COV(2, 2));
        printf("# chisq = %g\n", chisq);
    }
*/

    std::vector<double> coefficients = {gsl_vector_get(c, 0), gsl_vector_get(c, 1), gsl_vector_get(c, 2)};

    gsl_matrix_free(X);
    gsl_vector_free(y);
    gsl_vector_free(w);
    gsl_vector_free(c);
    gsl_matrix_free(cov);

    return std::make_tuple(approximation::codes::POSITIVE, coefficients);
}
