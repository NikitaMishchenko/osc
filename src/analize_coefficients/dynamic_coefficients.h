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

        namespace
        {
            bool correctRangesAndWarn(const size_t maxSize,
                                  size_t& fromIndex,
                                  size_t& finishIndex,
                                  size_t& windowWidth,
                                  size_t& stepSize)
            { 
                std::cout << "correctRangesAngWarnentry()\n";

                bool wasCorrected = false;

                if (fromIndex > maxSize)
                {
                    std::cerr << "\tfromIndex > max size, fromIndex to 0, "
                              << fromIndex << " > " << maxSize << "\n";    
                    
                    fromIndex = 0;

                    std::cerr << "\tfromIndex has beeng set to: " << fromIndex << "\n";

                    wasCorrected = true;
                }

                if (finishIndex > maxSize)
                {
                    std::cerr << "\tfinishIndex > max size, setting finishIndex to maxSize, "
                              << finishIndex << " > " << maxSize << "\n";    
                    
                    finishIndex = maxSize;

                    std::cerr << "\tfinishIndex has beeng set to: " << finishIndex << "\n";

                    wasCorrected = true;
                }

                if (fromIndex + windowWidth > maxSize)
                {
                    std::cerr << "fromIndex + windowWidth > max size, setting windowWidth to maximum possible, "
                              << fromIndex + windowWidth << " > " << maxSize << "\n";    
                    
                    windowWidth = maxSize - fromIndex;
                    
                    std::cerr << "\twindowWidth has beeng set to: " << windowWidth << "\n";

                    wasCorrected = true;
                }

                if (fromIndex + stepSize > maxSize)
                {
                    std::cerr << "\tfromIndex + stepSize > max size, setting stepSize to maximum possible, "
                              << fromIndex + stepSize << " > " << maxSize << "\n";

                    stepSize = maxSize - fromIndex;

                    std::cerr << "\tstepSize has beeng set to: " << stepSize << "\n";

                    wasCorrected = true;
                }

                return wasCorrected;
            }

            // todo move to linnear_apptroximation
            bool isRangesCorrect(const size_t maxSize,
                                 const size_t fromIndex,
                                 const size_t finishIndex,
                                 const size_t windowWidth,
                                 const size_t stepSize)
            {
                std::cout << "isRangesCorrect entry()\n";

                if ((fromIndex > maxSize) || (finishIndex > maxSize) || (fromIndex + windowWidth > maxSize) || (fromIndex + stepSize > maxSize))
                    return false;
                
                return true;    
            }

        } // namespace

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
        std::tuple<int, linnear_approximation::ApproxResultVector> calcMzEqvivalentCoefficient(const size_t indexFromData, 
                                                                                         const size_t indexToData,
                                                                                         const size_t windowSize,
                                                                                         const size_t stepSize)
        {
            std::cout << "calcMzEqvivalentCoefficient()\n";

            m_wtOscillation.getAngleAmplitudeIndexes();

            //m_wtOscillation.info();

            int resultCode;
            linnear_approximation::ApproxResultVector approxResultVector;

            {
                m_wtOscillation.write("takeAmplFromThis");

                std::vector<double> x = m_wtOscillation.getTimeAmplitude();
                std::vector<double> y = m_wtOscillation.getAngleAmplitude();
                if (x.size() != x.size())
                    throw "err ranges in approx preparation";

                
                size_t FROM_INDEX_X = indexFromData; // 400; // начинаем с

                size_t FINISH_INDEX = indexToData; //10000; // кончаем

                size_t WINDOW_WIDTH = windowSize; //100; // апрроксимация на
                size_t STEP_SIZE = stepSize; //WINDOW_WIDTH * 1;    

                // based on data
                const size_t MAX_SIZE_DATA = x.size();

                correctRangesAndWarn(MAX_SIZE_DATA, FROM_INDEX_X, FINISH_INDEX, WINDOW_WIDTH, STEP_SIZE);

                {
                    // PARAMETERS FOR STEP
                    size_t stepFromIdx = FROM_INDEX_X;
                    size_t stepToIdx = stepFromIdx + WINDOW_WIDTH; 

                    linnear_approximation::ApproxResult approxResult;

                    for ( ; stepToIdx < FINISH_INDEX;)
                    {
                        std::vector<double> subX(x.begin() + stepFromIdx, x.begin() + stepToIdx);
                        std::vector<double> subY(y.begin() + stepFromIdx, y.begin() + stepToIdx);

                        // prepare for approx
                        for (auto &itm : subY)
                        {
                            itm = abs(itm);
                        }

                        // AngleHistory a(subX, subY);
                        // a.write("approxThis");

                        // fixme subY = log(subY);

                        std::tie(resultCode, approxResult) = linnear_approximation::approximate(subX, subY);

                        approxResultVector.emplace_back(approxResult);
                        std::cout << "approxResultVector.size: " << approxResultVector.size() << "\n";

                        stepFromIdx += STEP_SIZE;
                        stepToIdx += STEP_SIZE;
                    }

                }
            }
            std::cout << "performed\n";

            // manage features
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

        /** Коэффициент, обезразмеривающий динамический коэффициент
         */
        double m_dimensionOfCoefficient; // 2I/T
    };

}
