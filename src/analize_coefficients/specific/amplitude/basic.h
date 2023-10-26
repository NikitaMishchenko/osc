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
        DEFAULT_AMPLITUDE,
        ABS_AMPLITUDE
        // todo other mode
    };

    struct AngleAmplitudeBase // todo rename AngleAmplitudeEssential
    {
        AngleAmplitudeBase() : m_amplitudeTime(0),
                               m_amplitudeAngle(0),
                               m_amplitudeDangle(0),
                               m_frequency(0),
                               m_amplitudeIndexesFromInitialAngle(-1) {}

        AngleAmplitudeBase(double time, double angle, int index)
            : m_amplitudeTime(time),
              m_amplitudeAngle(angle),
              m_amplitudeIndexesFromInitialAngle(index)
        {
        }

        AngleAmplitudeBase(double time, double angle, double dangle, double frequency, int index)
            : m_amplitudeTime(time),
              m_amplitudeAngle(angle),
              m_amplitudeDangle(0),
              m_frequency(0),
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

        friend std::ostream &operator<<(std::ostream &out, const AngleAmplitudeBase &input)
        {
            out << input.m_amplitudeTime << " "
                << input.m_amplitudeAngle << " "
                << input.m_amplitudeDangle << " "
                << input.m_frequency << " "
                << input.m_amplitudeIndexesFromInitialAngle << "\n";

            return out;
        }

        double m_amplitudeTime;
        double m_amplitudeAngle;
        double m_amplitudeDangle;
        double m_frequency; //< from periods
        int m_amplitudeIndexesFromInitialAngle;

    private:
    };

    /**
     * Get amplitude of oscilating signal
     **/
    class AngleAmplitudeVector
    {
    public:
        AngleAmplitudeVector()
        {
        }

        AngleAmplitudeVector(const std::shared_ptr<std::vector<double>> time,
                             const std::shared_ptr<std::vector<double>> angle,
                             const std::shared_ptr<std::vector<double>> dangle,
                             int mode = ABS_AMPLITUDE)// : m_time(time),
                                                         //m_angle(angle),
                                                         //m_dangle(dangle)
        {
            initialize(time, angle, dangle);
        }

        bool initialize(const std::shared_ptr<std::vector<double>> time,
                        const std::shared_ptr<std::vector<double>> angle,
                        const std::shared_ptr<std::vector<double>> dangle,
                        int mode = ABS_AMPLITUDE)
        {
            m_time = time;
            m_angle = angle;
            m_dangle = dangle;

            bool isOk = doWork(mode);

            if (!isOk)
                return isOk;

            isOk = calculateFrequency();

            if (!isOk)
                return isOk;

            return isOk;
        }

        bool doWork(const int mode)
        {
            if (m_time->size() != m_angle->size() || m_angle->size() != m_dangle->size())
            {
                std::string msg = "AngleAmplitude: size of time, angle, dangle not equal!";
                msg += m_time->size();
                msg += m_angle->size();
                throw(std::runtime_error(msg));
            }

            if (m_angle->size() < 1)
                return false;

            std::vector<double> tmpDangle = *m_dangle;
            //tmpDangle = actLinnearGaussFilter(50, 1, tmpDangle);

            for (size_t i = 1; i < tmpDangle.size(); i++)
            {
                if (tmpDangle.at(i - 1) <= 0 && tmpDangle.at(i) > 0)
                    m_AngleAmplitudeIndexes.emplace_back(i);

                if (tmpDangle.at(i - 1) >= 0 && tmpDangle.at(i) < 0)
                    m_AngleAmplitudeIndexes.emplace_back(i);
            }

            // get area arround amplitude index
            const int areaSize = 3;
            // approximate via ax^2+b*x+c -> x_extrenum = -b/(2a) y_extrenum= a(x_extrenum)^2+b(x_extrenum)+c

            m_angleAmplitudeBase.reserve(m_AngleAmplitudeIndexes.size());

            for (const auto &index : m_AngleAmplitudeIndexes)
            {
                // int amplitudeIndex = m_AngleAmplitudeIndexes.at(index);
                m_angleAmplitudeBase.emplace_back(
                    AngleAmplitudeBase(m_time->at(index),
                                       (ABS_AMPLITUDE == mode) ? std::abs(m_angle->at(index)) : m_angle->at(index),
                                       m_dangle->at(index),
                                       0,
                                       index));
            }

            return true;
        }

        bool calculateFrequency()
        {
            for (std::vector<AngleAmplitudeBase>::iterator amplitudeData = m_angleAmplitudeBase.begin();
                 amplitudeData != m_angleAmplitudeBase.end() - 2;
                 amplitudeData++)
            {
                // m_freqFromPeriod m_frequency
                // amplitudeData->m_frequency = 1.0 / ((amplitudeData + 2)->m_amplitudeTime - amplitudeData->m_amplitudeTime);
            }

            return true;
        }

        void sortViaTime()
        {
            std::sort(m_angleAmplitudeBase.begin(), m_angleAmplitudeBase.end(), [](AngleAmplitudeBase a, AngleAmplitudeBase b)
                      { return a.m_amplitudeTime < b.m_amplitudeTime; });
        }

        AngleAmplitudeBase getMaxAmplitude() const
        {
            auto it = std::max_element(m_angleAmplitudeBase.begin(), m_angleAmplitudeBase.end());

            if (it != m_angleAmplitudeBase.end())
                return *it;

            return AngleAmplitudeBase();
        }

        AngleAmplitudeBase getMinAmplitude() const
        {
            auto it = std::min_element(m_angleAmplitudeBase.begin(), m_angleAmplitudeBase.end());

            if (it != m_angleAmplitudeBase.end())
                return *it;

            return AngleAmplitudeBase();
        }

        size_t size() const { return m_AngleAmplitudeIndexes.size(); }
        double at(size_t index) const { return m_AngleAmplitudeIndexes.at(index); }

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

        void saveData(const std::string &fileName) const
        {
            std::ofstream fout(fileName);

            fout << *this;
        }

        friend std::ostream &operator<<(std::ostream &out, const AngleAmplitudeVector &input)
        {
            for (const auto &ampl : input.m_angleAmplitudeBase)
                out << ampl;

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
