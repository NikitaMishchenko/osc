#pragma once

#include <cmath>

#include <vector>

// todo research https://www.boost.org/doc/libs/1_56_0/libs/math/doc/html/math_toolkit/internals2/minimax.html
// #include <boost/math/tools/polynomial.hpp>
// boost::math::tools::polynomial<double> p;

#include "../oscillation/wt_oscillation.h"
#include "../model/tr_rod_model_params.h"

namespace dynamic_coefficients
{
    enum APPROACH_DYNAMIC_COEFFICIENTS_COEFFICIENTS
    {
        LOGARITHM_AND_REDD
    };

    struct DampingCoefficients
    {
        std::vector<double> eqvivalentDamp;
        std::vector<double> actualDamp;
    };

    class EqvivalentDamping
    {
    public:
        EqvivalentDamping(const WtOscillation &wtOscillation, const Model &model)
            : m_wtOscillation(wtOscillation),
              m_dimensionOfCoefficient(2.0 * model.getI() / wtOscillation.getTimeStamp()){};

        bool doCalc(APPROACH_DYNAMIC_COEFFICIENTS_COEFFICIENTS approach)
        {
            switch (approach)
            {
            case LOGARITHM_AND_REDD:
            {
                m_coeff.eqvivalentDamp = calcLogAmplitude();

                // todo polynomial approximation to calculate DampingCoefficients::actualDamping

                return true;
            }

            default:
                return false;
            }
        }

    private:
        std::vector<double> calcLogAmplitude()
        {
            std::vector<double> res;

            res.reserve(m_wtOscillation.getAngle().size() - 1);

            for (size_t i = 0; i < m_wtOscillation.getAngle().size() - 1; ++i)
            {
                res.push_back(m_dimensionOfCoefficient 
                              * log(m_wtOscillation.getAngle().at(i + 1) / m_wtOscillation.getAngle().at(i + 1)));
            }

            return res;
        }

        std::vector<double> calcMzDynamic()
        {
            std::vector<double> res;

            // approx amplitude 

            return res;
        }


        WtOscillation m_wtOscillation;
        DampingCoefficients m_coeff;
        std::vector<double> coefficient;

        /** Коэффициент, обезразмеривающий динамический коэффициент
         */
        double m_dimensionOfCoefficient; // 2I/T
    };

    class ActualDamping
    {

    public:
        ActualDamping(const EqvivalentDamping &EqvivalentDamping)
        {
            reddProcedure();
        };

    private:
        void reddProcedure(){

        };
    };
}
