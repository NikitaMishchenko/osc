#pragma once

#include <cmath>
#include <cassert>

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
         *   result = c1*t+c0
         *   y = c1*x + c0
         *   m_wtOscillation->getAmplitude
         *   y = ln(amplitude) = ln(m_wtOscillation.getAmplitude())
         *   x = m_wtOscillation.getTime();
         *   approximate (x , y)
         *   c1 = ln(c)
         *   c0 = n_c
         */
        std::tuple<int, linnear_approximation::ApproxResult> calcMzEqvivalentCoefficient()
        {
            std::cout << "calcMzEqvivalentCoefficient()\n";

            m_wtOscillation.getAngleAmplitudeIndexes();

            m_wtOscillation.info();

            // todo assert(!AngleAmplitudeIndexes.empty());

            int resultCode;
            linnear_approximation::ApproxResult approxResult;

            {
                m_wtOscillation.write("takeAmplFromThis");

                std::vector<double> x = m_wtOscillation.getTimeAmplitude();
                std::vector<double> y = m_wtOscillation.getAngleAmplitude();

                {

                    //  берем часть, для тестов
                    // todo range
                    size_t fromIdx, toIdx;
                    double offset = 0.1;

                    fromIdx = int(x.size()*offset);
                    toIdx = x.size() - fromIdx;

                    std::vector<double> subX(x.begin() + fromIdx, x.begin() + toIdx);
                    std::vector<double> subY(y.begin() + fromIdx, y.begin() + toIdx);

                    for (auto &itm : subY)
                    {
                        itm = abs(itm);
                    }

                    AngleHistory a(subX, subY);
                    a.write("approxThis");

                    // fixme subY = log(subY);

                    std::tie(resultCode, approxResult) = linnear_approximation::approximate(subX, subY);

                    std::cout << "AproxxResult:\n"
                              << "c0: " << approxResult.c0 << "\n"
                              << "c1: " << approxResult.c1 << "\n"
                              << "cov00: " << approxResult.cov00 << "\n"
                              << "cov01: " << approxResult.cov01 << "\n"
                              << "cov11: " << approxResult.cov11 << "\n"
                              << "chisq: " << approxResult.chisq << "\n";
                }
            }
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
        std::vector<double> calcLogAmplitude() const // fixme it's calculated in m_mzAmplitudeIndexes
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
