#pragma once

#include <cmath>

#include <vector>
#include <tuple>

#include <boost/optional.hpp>

// todo research https://www.boost.org/doc/libs/1_56_0/libs/math/doc/html/math_toolkit/internals2/minimax.html
// #include <boost/math/tools/polynomial.hpp>
// boost::math::tools::polynomial<double> p;

#include "../oscillation/wt_oscillation.h"
#include "../model/tr_rod_model_params.h"
#include "../gnusl_proc/linnear_approximation.h"

namespace
{
    std::vector<double> log(const std::vector<double> &arg)
    {
        std::vector<double> result;

        result.reserve(arg.size());

        for (size_t i = 0; i < arg.size(); ++i)
        {
            result.emplace_back(std::log(arg.at(i)));
        }

        return result;
    }
}

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
        EqvivalentDamping(const WtOscillation &wtOscillation)
            : m_wtOscillation(wtOscillation),
              m_dimensionOfCoefficient(2.0 * m_wtOscillation.getModel().getI() / wtOscillation.getTimeStamp())
        {
            std::cout << "EqvivalentDamping ctr()\n";
        };

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

        std::vector<double> getMzEqvivalent() const
        {
            return m_coeff.eqvivalentDamp;
        }

        /*
         *   ln(Theta(t)) = (n_c(Theta_sr)*t) + ln(c)
         *   m_wtOscillation->getAmplitude
         *   y = ln(amplitude)
         *   x = m_wtOscillation.getTime();
         *   c1 = ln(c)
         *   c0 = n_c
         */
        std::tuple<int, linnear_approximation::ApproxResult> calcMzEqvivalentCoefficient()
        {
            std::cout << "calcMzEqvivalentCoefficient()\n";

            std::vector<double> res;

            int resultCode;
            linnear_approximation::ApproxResult approxResult;

            std::tie(resultCode, approxResult) = linnear_approximation::approximate(m_wtOscillation.getAngleAmplitude(),
                                                                                    log(m_wtOscillation.getTimeAmplitude()),
                                                                                    boost::optional<std::vector<double>>());

            std::cout << "performed\n";

            approxResult.save("outputApproxResult");
            
            // manage features
            return std::make_tuple(resultCode, approxResult);
        }

    private:
        /*
         *   Theta(t) = c*exp(-n_c(Theta_sr)*t)
         *   ln(Theta(t)) = (n_c(Theta_sr)*t) + ln(c)
         */
        std::vector<double> calcLogAmplitude() // fixme it's calculated in m_mzAmplitudeIndexes
        {
            std::cout << "calcLogAmplitude() ";

            std::vector<double> res;

            res.reserve(m_wtOscillation.getAngle().size() - 1);

            for (size_t i = 0; i < m_wtOscillation.getAngle().size() - 1; ++i)
            {
                res.push_back(m_dimensionOfCoefficient * log(m_wtOscillation.getAngle().at(i + 1) / m_wtOscillation.getAngle().at(i + 1)));
            }
            
            std::cout << res.size() << "\n";

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
