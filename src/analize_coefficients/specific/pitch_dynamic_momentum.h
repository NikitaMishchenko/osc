#pragma once

#include <iostream>

#include <memory>
#include <vector>

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
        if (time->size() == angle->size() && angle->size() == dangle->size())
            throw(std::runtime_error("AngleAmplitude: size of time, angle, dangle not equal!"));

        doWork();
    }

    std::vector<AngleAmplitudeBase> m_angleAmplitudeBase;

private:
    void doWork()
    {
        for (size_t index = 0; index < m_angle->size(); ++index)
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
    }

    std::shared_ptr<std::vector<double>> m_time;
    std::shared_ptr<std::vector<double>> m_angle;
    std::shared_ptr<std::vector<double>> m_dangle;
};

// numberOfPeriods количество периодов колебаний
class PitchDynamicMomentum
{
public:
    PitchDynamicMomentum(std::shared_ptr<std::vector<double>> time,
                         std::shared_ptr<std::vector<double>> angle,
                         std::shared_ptr<std::vector<double>> dangle,
                         const int numberOfPeriods = 2) : m_time(time),
                                                          m_angle(dangle),
                                                          m_dangle(dangle),
                                                          m_nP(numberOfPeriods),
                                                          m_angleAmplitude(time, angle, dangle)
    {}

    // at max amplitude
    bool calculate()
    {
        std::vector<AngleAmplitudeBase>::iterator it = m_angleAmplitude.m_angleAmplitudeBase.begin();

        std::vector<double> dataToApproximateY;
        std::vector<double> dataToApproximateX;
        std::vector<approximation::nonlinnear::ApproximationResult> approximationResultVector;

        while (m_angleAmplitude.m_angleAmplitudeBase.end() != it)
        {
            for (int i = 0; i < m_nP * 2; i++, m_angleAmplitude.m_angleAmplitudeBase.end() != it)
            {
                dataToApproximateX.push_back(it->m_amplitudeTime);
                dataToApproximateY.push_back(it->m_amplitudeAngle);
                it++;
            }

            if (dataToApproximateY.size() < 2)
                break;

            int errCode;
            approximation::nonlinnear::ApproximationResult approximationResult;

            std::tie(errCode, approximationResult) = approximation::nonlinnear::approximate(dataToApproximateX, dataToApproximateY);

            dataToApproximateX.clear();
            dataToApproximateY.clear();

            if (0 != errCode)
                continue;

            approximationResultVector.push_back(approximationResult);
        }

        return true;
    }

private:
    // INPUT
    std::shared_ptr<std::vector<double>> m_time;
    std::shared_ptr<std::vector<double>> m_angle;
    std::shared_ptr<std::vector<double>> m_dangle;

    // settings
    int m_nP; // количество периодов колебаний

    // INTERNAL
    std::vector<double> m_pitchStaticMomentum;

    AngleAmplitude m_angleAmplitude;
};
