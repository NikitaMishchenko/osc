#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include <algorithm>

#include "gnusl_wrapper/filters/gauissian.h"

namespace amplitude
{
    enum Mode
    {
        ABS_AMPLITUDE
        // todo other mode
    };

    struct AngleAmplitudeBase // todo rename AngleAmplitudeEssential
    {
        AngleAmplitudeBase() : m_amplitudeTime(0),
                               m_amplitudeAngle(0),
                               m_amplitudeIndexesFromInitialAngle(-1) {}

        AngleAmplitudeBase(double time, double angle, int index) : m_amplitudeTime(time),
                                                                   m_amplitudeAngle(angle),
                                                                   m_amplitudeIndexesFromInitialAngle(index)
        {
        }

        bool operator==(const AngleAmplitudeBase &r) const
        {
            return m_amplitudeAngle == r.m_amplitudeAngle;
        }

        bool operator<(const AngleAmplitudeBase &r) const
        {
            return m_amplitudeAngle < r.m_amplitudeAngle;
        }

        bool operator>(const AngleAmplitudeBase &r) const
        {
            return m_amplitudeAngle > r.m_amplitudeAngle;
        }

        double m_amplitudeTime;
        double m_amplitudeAngle;
        int m_amplitudeIndexesFromInitialAngle;

    private:
    };

    /**
     * Get amplitude of oscilating signal
     **/
    class AngleAmplitude
    {
    public:

        AngleAmplitude(const std::shared_ptr<std::vector<double>> time,
                       const std::shared_ptr<std::vector<double>> angle,
                       const std::shared_ptr<std::vector<double>> dangle) : m_time(time),
                                                                            m_angle(angle),
                                                                            m_dangle(dangle)
        {
            if (time->size() != angle->size() || angle->size() != dangle->size())
            {
                std::string msg = "AngleAmplitude: size of time, angle, dangle not equal!";
                msg += time->size();
                msg += angle->size();
                throw(std::runtime_error(msg));
            }

            doWork(ABS_AMPLITUDE); // fixme
        }

        bool doWork(const int mode)
        {
            if (m_angle->size() < 1)
                return false;

            std::vector<double> tmpDangle = *m_dangle;
            //tmpDangle = actLinnearGaussFilter(50, 1, tmpDangle);

            std::ofstream fout("/home/mishnic/data/phd/sphere_cone_M1.75/ah_am");

            for (size_t i = 1; i < tmpDangle.size(); i++)
            {
                if (tmpDangle.at(i - 1) <= 0 && tmpDangle.at(i) > 0)
                    m_AngleAmplitudeIndexes.emplace_back(i);

                if (tmpDangle.at(i - 1) >= 0 && tmpDangle.at(i) < 0)
                    m_AngleAmplitudeIndexes.emplace_back(i);

                fout << m_time->at(i) << " " << m_angle->at(i) << " " << m_dangle->at(i) << "\n";
            }


            m_angleAmplitudeBase.reserve(m_AngleAmplitudeIndexes.size());

            for (const auto& index : m_AngleAmplitudeIndexes)
            {
                //int amplitudeIndex = m_AngleAmplitudeIndexes.at(index);
                m_angleAmplitudeBase.emplace_back(
                    AngleAmplitudeBase(m_time->at(index),
                                       (ABS_AMPLITUDE == mode) ? std::abs(m_angle->at(index)) : m_angle->at(index),
                                       index));
            }

            /*for (size_t index = 0; index < m_angle->size() - 1; ++index)
            {
                // pick at amplitude extremum
                if ((tmpDangle.at(index) <= 0 && tmpDangle.at(index + 1) >= 0) ||
                    (tmpDangle.at(index) >= 0 && tmpDangle.at(index + 1) <= 0))
                {
                    index = getActualMax(index);

                    m_angleAmplitudeBase.push_back(
                        AngleAmplitudeBase(m_time->at(index),
                        (ABS_AMPLITUDE == mode) ? std::abs(m_angle->at(index)) : m_angle->at(index),
                        index));
                }
            }*/

            return true;
        }

        void sortViaTime()
        {
            std::sort(m_angleAmplitudeBase.begin(), m_angleAmplitudeBase.end(), [](AngleAmplitudeBase a, AngleAmplitudeBase b)
                      {
                          return a.m_amplitudeTime < b.m_amplitudeTime;
                      });
        }

    private:
        size_t getActualMax(const size_t index, const size_t lookGap = 1) const
        {
            if (index > lookGap)
            {
                double d0 = std::abs(m_angle->at(index - 1));
                double d1 = std::abs(m_angle->at(index));
                if (d0 > d1)
                {
                    return index - 1;
                }
            }

            if (m_angle->size() - lookGap < index)
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
                fout << m_angleAmplitudeBase.at(i).m_amplitudeTime << "\t"
                     << m_angleAmplitudeBase.at(i).m_amplitudeAngle << "\t"
                     << m_angleAmplitudeBase.at(i).m_amplitudeIndexesFromInitialAngle << "\n";
        }

        friend std::ostream &operator<<(std::ostream &out, const AngleAmplitude &input)
        {
            for (const auto& ampl : input.m_angleAmplitudeBase)
                out << ampl.m_amplitudeTime << " "
                    << ampl.m_amplitudeAngle << " "
                    << ampl.m_amplitudeIndexesFromInitialAngle << "\n";
            
            return out;
        }

    public:
        std::vector<AngleAmplitudeBase> m_angleAmplitudeBase;

    private:
        std::shared_ptr<std::vector<double>> m_time;
        std::shared_ptr<std::vector<double>> m_angle;
        std::shared_ptr<std::vector<double>> m_dangle;
        std::vector<int> m_AngleAmplitudeIndexes;
    };
} // namespace amplitude
