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
inline void generateTestSignal(gsl_vector *x, const size_t totalSize)
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

            std::vector<double> filterGaussian(const std::vector<double>& inputData,
                                gsl_filter_end_t type = GSL_FILTER_END_PADVALUE)
            {
                Vector resultGsl(inputData.size());
                Vector inputGsl(inputData);

                gsl_filter_gaussian(type, m_alpha, 0, inputGsl.getGslVector(), resultGsl.getGslVector(), m_gauss.get());

                return resultGsl.getStlVector();
            }

        private:
            GslGaussian m_gauss;
            Vector m_kernal;
            size_t m_windowSize;
            double m_alpha;
        };

    } // namesapce filters
} // namespace gsl_wrapper

inline std::vector<double> actLinnearGaussFilter(const size_t windowSize,
                                 const double alpha,
                                 const std::vector<double> &dataVector)
{
    
    gsl_wrapper::filters::GslGaussian gauss(windowSize);

    gsl_wrapper::filters::GaussianPerformer gaussianPerformer(alpha, windowSize);
        
    std::vector<double> reusltAlpha = gaussianPerformer.filterGaussian(dataVector);

    return reusltAlpha;
}
