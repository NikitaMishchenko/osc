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

// todo make template class
class FourVector
{
public:
    FourVector()
    {
    }

    FourVector(const size_t size) : m_d0(size), m_d1(size), m_d2(size), m_d3(size)
    {
    }

    FourVector(const std::vector<double> &v0,
               const std::vector<double> &v1,
               const std::vector<double> &v2,
               const std::vector<double> &v3)
        : m_d0(v0), m_d1(v1), m_d2(v2), m_d3(v3){};

    friend std::ostream &operator<<(std::ostream &foutSource, const FourVector &data)
    {
        // for (size_t i = data.size(); i < data.size()/2 + data.size()%2; ++i)
        for (size_t i = 0; i < data.size(); ++i)
        {
            // std::cout << data.m_0.at(i) << "\t" << data.m_1.at(i) << "\t" << data.m_2.at(i) << "\t" << data.m_3.at(i) << "\n";
            foutSource << data.m_d0.at(i)<< " " << data.m_d1.at(i) << " " << data.m_d2.at(i) << " " << data.m_d3.at(i) << "\n";
        }
        return foutSource;
    }

    void reserve(const size_t size)
    {
        m_d0.reserve(size);
        m_d1.reserve(size);
        m_d2.reserve(size);
        m_d3.reserve(size);
    }

    void push_back(double d0, double d1, double d2, double d3)
    {
        m_d0.push_back(d0);
        m_d1.push_back(d1);
        m_d2.push_back(d2);
        m_d3.push_back(d3);
    }

    size_t size() const
    {
        return m_d1.size();
    }

    std::vector<double> m_d0;
    std::vector<double> m_d1;
    std::vector<double> m_d2;
    std::vector<double> m_d3;
};

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

namespace gsl_to_stl
{
    class GslWrapper
    {
    public:
        GslWrapper(const size_t size) : m_sizeGsl(size), m_initialised(false)
        {
            std::cout << "GslWrapper ctr() empty() size: " << m_sizeGsl << "\n";

            m_gsl = gsl_vector_alloc(m_sizeGsl);
        }

        GslWrapper(const std::vector<double> &stlVector) : m_sizeGsl(stlVector.size()), m_initialised(true)
        {
            std::cout << "GslWrapper ctr() via std::cector size: " << m_sizeGsl << "\n";

            m_gsl = gsl_vector_alloc(m_sizeGsl);

            for (size_t i = 0; i < m_sizeGsl; ++i)
                gsl_vector_set(m_gsl, i, stlVector.at(i));
        }

        ~GslWrapper()
        {
            gsl_vector_free(m_gsl);
        }

        std::vector<double> getStlVector()
        {
            if (!m_initialised)
                initialiseStl();

            return m_stlVector;
        }

        gsl_vector *getGslVector() // what about const would it work ok in const usage cases?
        {
            m_initialised = false;

            return m_gsl;
        }

        double getGsl(size_t i)
        {
            m_initialised = false;

            return double(gsl_vector_get(m_gsl, i));
        }

    private:
        void initialiseStl()
        {
            std::cout << "initialiseStl()\n";

            m_stlVector.reserve(m_sizeGsl);

            for (size_t i = 0; i < m_sizeGsl; ++i)
                m_stlVector.push_back(gsl_vector_get(m_gsl, i));
        }

        std::vector<double> m_stlVector; // mutable
        gsl_vector *m_gsl;
        size_t m_sizeGsl;
        bool m_initialised; // mutable
    };

    void gslVectorFromStlVector(gsl_vector *gslVector, const size_t gslVectorSize, const std::vector<double> &stlVector)
    {
        // gsl_vector *x = gsl_vector_alloc(stlVector.size()); /* input vector */

        if (stlVector.size() < gslVectorSize)
            throw "data vector size() for GaussFilter err ";

        for (size_t i = 0; i < gslVectorSize; ++i)
        {
            gsl_vector_set(gslVector, i, stlVector.at(i));
        }
    }

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

    } // namesapce filters
} // namespace gsl_to_stl

FourVector actLinnearGaussFilter(const size_t windowSize,
                                 const double alpha1,
                                 const double alpha2,
                                 const double alpha3,
                                 const std::vector<double> &dataVector)
{
    const size_t totalSize = dataVector.size();

    gsl_to_stl::GslWrapper x(dataVector); // input

    gsl_to_stl::GslWrapper y1(totalSize);
    gsl_to_stl::GslWrapper y2(totalSize);
    gsl_to_stl::GslWrapper y3(totalSize);

    gsl_to_stl::filters::GslGaussian gauss(windowSize);

    {
        // internal
        gsl_to_stl::GslWrapper k1(windowSize); /* Gaussian kernel for alpha1 */
        gsl_to_stl::GslWrapper k2(windowSize);
        gsl_to_stl::GslWrapper k3(windowSize);
        std::cout << "kernels allocated\n";

        // todo move that close to gsl_gaussian_workspacel
        /* compute kernels without normalization */
        gsl_filter_gaussian_kernel(alpha1, 0, 0, k1.getGslVector());
        gsl_filter_gaussian_kernel(alpha2, 0, 0, k2.getGslVector());
        gsl_filter_gaussian_kernel(alpha3, 0, 0, k3.getGslVector());
        // params seems no sence with initial example data

        std::cout << "kernels computed\n";

        /* apply filters */
        gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha1, 0, x.getGslVector(), y1.getGslVector(), gauss.get());
        gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha2, 0, x.getGslVector(), y2.getGslVector(), gauss.get());
        gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha3, 0, x.getGslVector(), y3.getGslVector(), gauss.get());
        // params seems no sence with initial example data

        std::cout << "filters applyed\n";

        /* print kernels */
        for (size_t i = 0; i < windowSize; ++i)
        {
            double k1i = k1.getGsl(i);
            double k2i = k2.getGsl(i);
            double k3i = k3.getGsl(i);
        }

        std::cout << "kernals printed\n";
    }

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
