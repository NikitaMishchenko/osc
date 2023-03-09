#pragma once

#include <memory>
#include <vector>


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

