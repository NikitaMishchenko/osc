#pragma once

#include <memory>
#include <vector>

enum Mode
{
    ABS_AMPLITUDE
    // todo other mode
};

struct AngleAmplitudeBase
{
    double m_amplitudeAngle;
    double m_amplitudeTime;
    int m_amplitudeIndexesFromInitialAngle;
};

class AngleAmplitude
{
public:
    AngleAmplitude()
    {
    }

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

    bool doWork(const int mode)
    {
        if (m_angle->size() < 1)
            return false;

        for (size_t index = 0; index < m_angle->size() - 1; ++index)
        {
            // pick at amplitude extremum
            if ((m_dangle->at(index) <= 0 && m_dangle->at(index + 1) >= 0) || 
                (m_dangle->at(index) >= 0 && m_dangle->at(index + 1) <= 0))
            {
                // index = getActualMax(index);

                AngleAmplitudeBase angleAmplitudeBase;

                angleAmplitudeBase.m_amplitudeIndexesFromInitialAngle = index;
                angleAmplitudeBase.m_amplitudeAngle = m_angle->at((ABS_AMPLITUDE == mode) ? std::abs(angleAmplitudeBase.m_amplitudeIndexesFromInitialAngle) : angleAmplitudeBase.m_amplitudeIndexesFromInitialAngle);
                angleAmplitudeBase.m_amplitudeTime = m_time->at(angleAmplitudeBase.m_amplitudeIndexesFromInitialAngle);

                m_angleAmplitudeBase.push_back(angleAmplitudeBase);
            }
        }

        std::cout << "saving amplitude size = " << m_angleAmplitudeBase.size() << "\n";
        saveData("amplitude");

        return true;
    }

private:
    size_t getActualMax(const size_t index) const
    {
        if (index > 1)
        {
            double d0 = std::abs(m_angle->at(index - 1));
            double d1 = std::abs(m_angle->at(index));
            if (d0 > d1)
            {
                return index - 1;
            }
        }

        if (m_angle->size() - 1 < index)
        {
            double d0 = std::abs(m_angle->at(index));
            double d1 = std::abs(m_angle->at(index + 1));
            if (d1 > d0)
            {
                return index + 1;
            }
        }

        return index;
    }

    void saveData(const std::string &fileName)
    {
        std::ofstream fout(fileName);

        for (size_t i = 0; i < m_angleAmplitudeBase.size(); i++)
            fout << m_angleAmplitudeBase.at(i).m_amplitudeAngle << "\t" << m_angleAmplitudeBase.at(i).m_amplitudeTime << "\t" << m_angleAmplitudeBase.at(i).m_amplitudeIndexesFromInitialAngle << "\n";
    }

    std::shared_ptr<std::vector<double>> m_time;
    std::shared_ptr<std::vector<double>> m_angle;
    std::shared_ptr<std::vector<double>> m_dangle;
};
