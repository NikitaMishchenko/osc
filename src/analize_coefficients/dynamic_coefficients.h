#pragma once

#include <cmath>
#include <cassert>

#include <vector>
#include <tuple>

#include <boost/optional.hpp>

// todo research https://www.boost.org/doc/libs/1_56_0/libs/math/doc/html/math_toolkit/internals2/minimax.html
// #include <boost/math/tools/polynomial.hpp>
// boost::math::tools::polynomial<double> p;

#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "gnusl_proc/linnear_approximation.h"

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

    // todo AmplitudeHistory : public AngleHistory() // move this method to public section
    // bugged a lot
    void iterativeAmplitudeRemove0Harmonic(AngleHistory& amplitude)
    {
        std::cout << "iterativeAmplitudeRemove0Harmonic entry()\n";

        double prevSum = 360; // max posible 360 - 0.01
        double sum = prevSum;
        double diff = prevSum, prevDiff = prevSum;

        size_t size = ((amplitude.size() % 2 == 0) ? amplitude.size() : (amplitude.size() - 1));

        for(int i = 0; i < 10; i++)
        {
            std::cout << "i: " << i << " ";

            for(size_t i = 0; i < size; ++i)
            {
                sum += amplitude.getAngle(i);
            }

            std::cout << " sum: " << sum;
            diff = sum - prevSum;

            std::cout << " diff: " << diff;

            if ( abs( abs(diff) - abs(prevDiff)) < 0.000001)
            {
                return;
            }

            prevDiff = diff;

            amplitude.codomainAdd(-1.0 *prevDiff);

            std::cout << "\n";
        }
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
              m_dimensionOfCoefficient(2.0 * m_wtOscillation.getModel().getI() / wtOscillation.getTimeStep())
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


        void prepareAmplitude(bool isWtOsc = false)
        {
            m_wtOscillation.calcAngleAmplitudeIndexes();
            calcAmplitudeData();

            if (false) // (isWtOsc)
            {
                iterativeAmplitudeRemove0Harmonic(m_amplitudeData); // todo move method
            }
        }

        void calcAmplitudeData()
        {
            AngleHistory newAmplitude(m_wtOscillation.getTimeAmplitude(), m_wtOscillation.getAngleAmplitude());

            m_amplitudeData = newAmplitude;

            // todo
            // if (m_aplitudeTime.size() != m_aplitudeAngle.size())
            //    throw "err ranges in calcAmplitudeData";
        }


        std::tuple<int, linnear_approximation::ApproxResultVector> calcLogAmplitudeLinnarApproxCoeff(const size_t indexFromData,
                                                                                                     const size_t indexToData,
                                                                                                     const size_t windowSize,
                                                                                                     const size_t stepSize)
        {
            // todo implementation

            int resultCode = 666;
            linnear_approximation::ApproxResultVector approxResultVector;

            {
                std::tie(resultCode, approxResultVector) = linnear_approximation::windowApproximation(indexFromData,
                                                                                                      indexToData,
                                                                                                      windowSize,
                                                                                                      stepSize,
                                                                                                      m_amplitudeData.getTime(),
                                                                                                      m_amplitudeData.getAngle());
            }

            return std::make_tuple(resultCode, approxResultVector);
        }


        std::tuple<int, linnear_approximation::ApproxResultVector> calcAmplitudeLinnarApproxCoeff(const size_t indexFromData,
                                                                                                  const size_t indexToData,
                                                                                                  const size_t windowSize,
                                                                                                  const size_t stepSize)
        {
            std::cout << "calcAmplitudeLinnarApproxCoeff()\n";

            int resultCode = 666;
            linnear_approximation::ApproxResultVector approxResultVector;

            {
                // m_wtOscillation.write("takeAmplFromThis");
                
                // fixme y = log(y);

                std::tie(resultCode, approxResultVector) = linnear_approximation::windowApproximation(indexFromData,
                                                                                                      indexToData,
                                                                                                      windowSize,
                                                                                                      stepSize,
                                                                                                      m_amplitudeData.getTime(),
                                                                                                      m_amplitudeData.getAngle());
            }

            return std::make_tuple(resultCode, approxResultVector);
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

        AngleHistory m_amplitudeData;
        // std::vector<double> m_aplitudeTime;
        // std::vector<double> m_aplitudeAngle;

        /** Коэффициент, обезразмеривающий динамический коэффициент
         */
        double m_dimensionOfCoefficient; // 2I/T
    };

}
