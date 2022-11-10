#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <iostream>
#include <memory>

#include <gsl/gsl_math.h>
#include <gsl/gsl_filter.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>

#include "core/gsl_wrapper.h"
#include "filtration/helpers.h"


// todo start here and fill with external Data // leave basic realisation for testing
void generateTestSignal(gsl_vector *x, const size_t totalSize)
{
    std::cout << "generateSignal() with size: " << totalSize << "\n";

    // failed to alloc x here

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

namespace gsl_wrapper
{

    namespace filters
    {
        class GslGaussian
        {
        public:
            GslGaussian(const size_t windowSize) : m_windowSize(m_windowSize)
            {
                std::cout << "GslGaussian ctr(), size: " << windowSize << "\n";
                m_gauss_p = gsl_filter_gaussian_alloc(windowSize); // не работает если m_windowSize
            }

            ~GslGaussian()
            {
                gsl_filter_gaussian_free(m_gauss_p);
            }

            gsl_filter_gaussian_workspace *get()
            {
                return m_gauss_p;
            }

        private:
            gsl_filter_gaussian_workspace *m_gauss_p;
            size_t m_windowSize;
        };

        class GaussianPerformer
        {
        public:
            GaussianPerformer(const double alpha, const size_t windowSize)
                : m_gauss(windowSize), m_kernal(windowSize), m_windowSize(windowSize), m_alpha(alpha)
            {}

            ~GaussianPerformer()
            {}

            void configure(const double alpha, const size_t windowSize) 
            {
                m_kernal.reallocate(windowSize); // todo init
                
                gsl_filter_gaussian_kernel(m_alpha, 0, 0, m_kernal.getGslVector());

                m_windowSize = windowSize;
                m_alpha = alpha;
            }

            void filterGaussian(Vector& inputData,
                                Vector& outputData,
                                gsl_filter_end_t type = GSL_FILTER_END_PADVALUE)
            {
                gsl_filter_gaussian(type, m_alpha, 0, inputData.getGslVector(), outputData.getGslVector(), m_gauss.get());
            }

        private:
            GslGaussian m_gauss;
            Vector m_kernal;
            size_t m_windowSize;
            double m_alpha;
        };

    } // namesapce filters
} // namespace gsl_wrapper

FourVector actLinnearGaussFilter(const size_t windowSize,
                                 const double alpha1,
                                 const double alpha2,
                                 const double alpha3,
                                 const std::vector<double> &dataVector)
{
    const size_t totalSize = dataVector.size();

    gsl_wrapper::Vector x(dataVector); // input

    gsl_wrapper::Vector y1(totalSize);
    gsl_wrapper::Vector y2(totalSize);
    gsl_wrapper::Vector y3(totalSize);

    gsl_wrapper::filters::GslGaussian gauss(windowSize);

    
        // internal
         /* Gaussian kernel for alpha1 */
        /*gsl_wrapper::Vector k1(windowSize);
        gsl_wrapper::Vector k2(windowSize);
        gsl_wrapper::Vector k3(windowSize);*/
        std::cout << "kernels allocated\n";

        // todo move that close to gsl_gaussian_workspacel
        /* compute kernels without normalization */
        /*
        gsl_filter_gaussian_kernel(alpha1, 0, 0, k1.getGslVector());
        gsl_filter_gaussian_kernel(alpha2, 0, 0, k2.getGslVector());
        gsl_filter_gaussian_kernel(alpha3, 0, 0, k3.getGslVector());*/
        // params seems no sence with initial example data

        // store
        gsl_wrapper::filters::GaussianPerformer gaussianPerformer(alpha1, windowSize);

        std::cout << "kernels computed\n";

        /* apply filters */
        /*
        gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha1, 0, x.getGslVector(), y1.getGslVector(), gauss.get());
        gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha2, 0, x.getGslVector(), y2.getGslVector(), gauss.get());
        gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha3, 0, x.getGslVector(), y3.getGslVector(), gauss.get());
        */
        // params seems no sence with initial example data

        gaussianPerformer.filterGaussian(x, y1);

        std::cout << "filters applyed\n";

        /* print kernels */
        /*for (size_t i = 0; i < windowSize; ++i)
        {
            double k1i = k1.getGsl(i);
            double k2i = k2.getGsl(i);
            double k3i = k3.getGsl(i);
        }
        */

        std::cout << "kernals printed\n";
    

    FourVector result;

    /* print filter results */
    for (size_t i = 0; i < totalSize; ++i)
    {
        result.push_back(gsl_vector_get(x.getGslVector(), i),
                         y1.getGsl(i),
                         y2.getGsl(i),
                         y3.getGsl(i));
    }

    std::cout << "result stored\n";

    return result;
}
