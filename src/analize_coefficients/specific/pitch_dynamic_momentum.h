#pragma once

#include <iostream>

#include <memory>
#include <vector>
#include <tuple>
#include <cstdlib>

#include "gnusl_proc/approximation/nonlinear.h"

struct AngleAmplitudeBase
{
    double m_amplitudeAngle;
    double m_amplitudeTime;
    int m_amplitudeIndexesFromInitialAngle;
};

class AngleAmplitude
{
public:
    AngleAmplitude(std::shared_ptr<std::vector<double>> time,
                   std::shared_ptr<std::vector<double>> angle,
                   std::shared_ptr<std::vector<double>> dangle) : m_time(time),
                                                                  m_angle(angle),
                                                                  m_dangle(dangle)
    {
        if (time->size() != angle->size() && angle->size() == dangle->size())
        {
            std::string msg = "AngleAmplitude: size of time, angle, dangle not equal!";
            msg += time->size();
            msg += angle->size();
            throw(std::runtime_error(msg));
        }
    }

    std::vector<AngleAmplitudeBase> m_angleAmplitudeBase;

    bool doWork()
    {
        if (m_angle->size() < 1)
            return false;

        for (size_t index = 0; index < m_angle->size()-1; ++index)
        {
            // pick at amplitude extremum
            if (m_dangle->at(index) <= 0 && m_dangle->at(index + 1) >= 0)
            {
                AngleAmplitudeBase angleAmplitudeBase;

                angleAmplitudeBase.m_amplitudeAngle = m_angle->at(index);
                angleAmplitudeBase.m_amplitudeTime = m_time->at(index);
                angleAmplitudeBase.m_amplitudeIndexesFromInitialAngle = index;

                m_angleAmplitudeBase.push_back(angleAmplitudeBase);
            }
        }

        return true;
    }

private:

    std::shared_ptr<std::vector<double>> m_time;
    std::shared_ptr<std::vector<double>> m_angle;
    std::shared_ptr<std::vector<double>> m_dangle;
};

enum Mode
{
    ABS_AMPLITUDE
    // todo other mode
};

enum Method
{
    METHOD_1 = 1, // direct amplitude approximation
    METHOD_2      // combined
};

class PitchDynamicMomentum
{
public:
    PitchDynamicMomentum(std::shared_ptr<std::vector<double>> time,
                         std::shared_ptr<std::vector<double>> angle,
                         std::shared_ptr<std::vector<double>> dangle,
                         std::shared_ptr<std::vector<double>> ddangle,
                         const int numberOfPeriods = 2,
                         const int mode = ABS_AMPLITUDE,
                         const int method = METHOD_1) : m_time(time),
                                                        m_angle(dangle),
                                                        m_dangle(dangle),
                                                        m_ddangle(ddangle),
                                                        m_numberOfPeriods(numberOfPeriods),
                                                        m_mode(mode),
                                                        m_method(method),
                                                        m_angleAmplitude(time, angle, dangle)
    {
    }

    // todo unittests
    bool calculateEqvivalentDampingCoefficients() 
    {
        if (!m_angleAmplitude.doWork())
            return false;

        bool isOk = false;
        std::vector<approximation::nonlinnear::ApproximationResult> eqvivalentDampingCoefficientVector;

        std::vector<double> dynamicDampingCoefficient;
        
        if (METHOD_1 == m_method)
        {
            // damping coeffiients assumed to be constant on each calcuilated point
            std::tie(isOk, eqvivalentDampingCoefficientVector) = calculateEqvivalentDampingCoefficient();

            // calcualating actual coefficients
            // m_dyn = -2I*n(Ampl)*V/q/s/l/l

            double coeff = 1; // -2.0*I*v/q/s/l/l // model flow

            for (const auto &eqvivalentDampingCoefficient : eqvivalentDampingCoefficientVector)
            {
                dynamicDampingCoefficient.push_back(eqvivalentDampingCoefficient.lambda);
            }

            m_pitchMomentumBasic = dynamicDampingCoefficient;
        }
        else if (METHOD_2 == m_method)
        {
            if (m_dangle->empty() || m_pitchStaticMomentum.empty()) // || (m_dangle.size() != m_pitchStaticMomentum.size()))
                return false;

            double coeff = 1; // l/v model, flow

            for (size_t i = 0; i < m_pitchMomentumBasic.size(); ++i)
            {
               dynamicDampingCoefficient.push_back(m_pitchStaticMomentum.at(i) * m_angle->at(i) + coeff * m_pitchMomentumBasic.at(i) * m_dangle->at(i)); 
            }
        }

        return isOk;
    }

    bool calcuatePitchStaticMomentum()
    {
        std::cout << "calcuatePitchStaticMomentum(): m_dangle.size() = " << m_dangle->size() << "\t" << "m_ddangle.size() = " << m_ddangle->size() << "\n";

        double M_fr = 0; // fixme 
        double coeffFriction = M_fr; // 1/q/s/l // model, flow
        double Iz = 1; // model

        if (m_dangle->empty() || m_ddangle->empty())
            return false;

        for (size_t i = 0; i < m_dangle->size()-1; i++)
        {
            if (m_dangle->at(i) <= 0 && m_dangle->at(i+1)<= 0)
                m_pitchStaticMomentum.push_back(Iz*m_ddangle->at(i) - coeffFriction);
        }

        return true;
    }

private:

    std::tuple<bool, std::vector<approximation::nonlinnear::ApproximationResult>>
    calculateEqvivalentDampingCoefficient()
    {
        std::vector<AngleAmplitudeBase>::const_iterator it = m_angleAmplitude.m_angleAmplitudeBase.begin();
        std::vector<double> dataToApproximateY;
        std::vector<double> dataToApproximateX;

        bool isOk = false;
        std::vector<approximation::nonlinnear::ApproximationResult> approximationResultVector;

        while (m_angleAmplitude.m_angleAmplitudeBase.end() != it)
        {
            for (size_t i = 0; i < m_numberOfPeriods * 2; i++, m_angleAmplitude.m_angleAmplitudeBase.end() != it)
            {
                dataToApproximateX.push_back(it->m_amplitudeTime);
                dataToApproximateY.push_back((ABS_AMPLITUDE == m_mode) ? std::abs(it->m_amplitudeAngle) : it->m_amplitudeAngle);
                it++;
            }

            if (dataToApproximateY.size() < 2)
                break;

            int errCode = approximation::codes::NEGATIVE;
            approximation::nonlinnear::ApproximationResult approximationResult;

            // Model Yi = A * exp(-lambda * t_i) + b
            std::tie(errCode, approximationResult) = approximation::nonlinnear::approximate(dataToApproximateX, dataToApproximateY);

            dataToApproximateX.clear();
            dataToApproximateY.clear();

            if (approximation::codes::POSITIVE != errCode)
                continue;

            approximationResultVector.push_back(approximationResult);
        }

        isOk = !approximationResultVector.empty();

        return std::make_tuple(isOk, approximationResultVector);
    }

    // INPUT
    std::shared_ptr<std::vector<double>> m_time;
    std::shared_ptr<std::vector<double>> m_angle;
    std::shared_ptr<std::vector<double>> m_dangle;
    std::shared_ptr<std::vector<double>> m_ddangle;

    // settings
    int m_numberOfPeriods; // количество периодов колебаний
    int m_mode;
    int m_method;

    // INTERNAL
    std::vector<double> m_pitchStaticMomentum;
    std::vector<double> m_pitchMomentumBasic;

    AngleAmplitude m_angleAmplitude;
};
