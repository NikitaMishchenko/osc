#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <iostream>

#include <gsl/gsl_math.h>
#include <gsl/gsl_filter.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>

// todo make template class
class FourVector
{
public:

    FourVector(const size_t size) : m_0(size), m_1(size), m_2(size), m_3(size)
    {}

    FourVector(const std::vector<double> &v0,
               const std::vector<double> &v1,
               const std::vector<double> &v2,
               const std::vector<double> &v3)
        : m_0(v0), m_1(v1), m_2(v2), m_3(v3)
        {};

    friend std::ostream &operator<<(std::ostream &foutSource, FourVector &data)
    {
        for (size_t i = 0; i < data.size(); ++i)
        {
            //std::cout << data.m_0.at(i) << "\t" << data.m_1.at(i) << "\t" << data.m_2.at(i) << "\t" << data.m_3.at(i) << "\n";
            foutSource << data.m_0.at(i) << "\t" << data.m_1.at(i) << "\t" << data.m_2.at(i) << "\t" << data.m_3.at(i) << "\n";
        }
        return foutSource;
    }

    void reserve(const size_t size)
    {
        m_0.reserve(size);
        m_1.reserve(size);
        m_2.reserve(size);
        m_3.reserve(size);
    }

    void pushBack(double d0, double d1, double d2, double d3)
    {
        m_0.push_back(d0);
        m_1.push_back(d1);
        m_2.push_back(d2);
        m_3.push_back(d3);
    }
    

    size_t size() const
    {
        return m_0.size();
    }

    std::vector<double> m_0;
    std::vector<double> m_1;
    std::vector<double> m_2;
    std::vector<double> m_3;
};

// todo start here and fill with external Data // leave basic realisation for testing
void generateSignal(gsl_vector *x , const size_t totalSize)
{
    std::cout << "generateSignal() with size: " << totalSize << "\n"; 

    //failed to alloc x here

    gsl_rng *r = gsl_rng_alloc(gsl_rng_default);

    double sum = 0.0;

    /* generate input signal */
    for (size_t i = 0; i < totalSize; ++i)
    {
        double ui = gsl_ran_gaussian(r, 1.0);
        sum += ui;
        gsl_vector_set(x, i, sum);
    }

    gsl_rng_free(r); 
}


FourVector actGaussFilter(const size_t totalSize, const size_t windowSize, const double alpha1, const double alpha2, const double alpha3)
{
    // const size_t N = 500;                     /* length of time series */
    // const size_t K = 51;                      /* window size */
    // const double alpha[3] = {0.5, 3.0, 10.0}; /* alpha values */

    gsl_vector *y1 = gsl_vector_alloc(totalSize);     /* filtered output vector for alpha1 */
    gsl_vector *y2 = gsl_vector_alloc(totalSize);     /* filtered output vector for alpha2 */
    gsl_vector *y3 = gsl_vector_alloc(totalSize);     /* filtered output vector for alpha3 */

    // internal
    gsl_vector *k1 = gsl_vector_alloc(windowSize);     /* Gaussian kernel for alpha1 */
    gsl_vector *k2 = gsl_vector_alloc(windowSize);     /* Gaussian kernel for alpha2 */
    gsl_vector *k3 = gsl_vector_alloc(windowSize);     /* Gaussian kernel for alpha3 */
    
    gsl_filter_gaussian_workspace *gauss_p = gsl_filter_gaussian_alloc(windowSize);
    
    gsl_vector *x = gsl_vector_alloc(totalSize);      /* input vector */
    generateSignal(x, totalSize);
    
    std::cout << "signal generated\n";
    
    /* compute kernels without normalization */
    gsl_filter_gaussian_kernel(alpha1, 0, 0, k1);
    gsl_filter_gaussian_kernel(alpha2, 0, 0, k2);
    gsl_filter_gaussian_kernel(alpha3, 0, 0, k3); 
    // params seems no sence with initial example data

    std::cout << "kernels computed\n";

    /* apply filters */
    gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha1, 0, x, y1, gauss_p);
    gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha2, 0, x, y2, gauss_p);
    gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha3, 0, x, y3, gauss_p);
    // params seems no sence with initial example data

    std::cout << "filters applyed\n";

    /* print kernels */
    for (size_t i = 0; i < windowSize; ++i)
    {
        double k1i = gsl_vector_get(k1, i);
        double k2i = gsl_vector_get(k2, i);
        double k3i = gsl_vector_get(k3, i);

        // todo store kernals
        // printf("%e %e %e\n", k1i, k2i, k3i);
    }

    std::cout << "kernals printed\n";

    printf("\n\n");

    std::vector<double> xi(totalSize);
    std::vector<double> y1i(totalSize);
    std::vector<double> y2i(totalSize);
    std::vector<double> y3i(totalSize);

    /* print filter results */
    for (size_t i = 0; i < totalSize; ++i)
    {
        /*double xi = gsl_vector_get(x, i);
        double y1i = gsl_vector_get(y1, i);
        double y2i = gsl_vector_get(y2, i);
        double y3i = gsl_vector_get(y3, i);*/

        xi.push_back(gsl_vector_get(x, i));
        y1i.push_back(gsl_vector_get(y1, i));
        y2i.push_back(gsl_vector_get(y2, i));
        y3i.push_back(gsl_vector_get(y3, i));

        // printf("%.12e %.12e %.12e %.12e\n", xi, y1i, y2i, y3i);
    }

    std::cout << "result stored\n";

    FourVector result(xi, y1i, y2i, y3i);
    std::cout << "Result size:" << result.size() << "\n";

    std::cout << "result packed to FourVector\n";

    gsl_vector_free(x);
    
    gsl_vector_free(y1);
    gsl_vector_free(y2);
    gsl_vector_free(y3);

    gsl_vector_free(k1);
    gsl_vector_free(k2);
    gsl_vector_free(k3);

    gsl_filter_gaussian_free(gauss_p);

    std::cout << "gsl containers freed\n";

    return result;
}