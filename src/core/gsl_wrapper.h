#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <iostream>
//#include <memory>

//#include <gsl/gsl_math.h>
//#include <gsl/gsl_filter.h>
//#include <gsl/gsl_rng.h>
//#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>



namespace gsl_wrapper
{
    class Vector
    {
    public:
        Vector(const size_t size) : m_sizeGsl(size), m_initialised(false)
        {
            std::cout << "GslWrapper ctr() empty() size: " << m_sizeGsl << "\n";

            m_gsl = gsl_vector_alloc(m_sizeGsl);
        }

        Vector(const std::vector<double> &stlVector) : m_sizeGsl(stlVector.size()), m_initialised(true)
        {
            std::cout << "GslWrapper ctr() via std::cector size: " << m_sizeGsl << "\n";

            m_gsl = gsl_vector_alloc(m_sizeGsl);

            for (size_t i = 0; i < m_sizeGsl; ++i)
                gsl_vector_set(m_gsl, i, stlVector.at(i));
        }

        ~Vector()
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

        // todo reallocate gsl vector memory
        void reallocate(const size_t size)
        {
            m_sizeGsl = size;
            m_gsl = gsl_vector_alloc(m_sizeGsl);

            m_stlVector.reserve(m_sizeGsl);
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

} // namespace gsl_wapper
