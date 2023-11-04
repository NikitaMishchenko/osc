#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>

#include "gnusl_wrapper/filters/gauissian.h"
#include "gnusl_wrapper/approximation/quadratic_approximation.h"

namespace amplitude
{
    enum Mode
    {
        DEFAULT_AMPLITUDE,
        ABS_AMPLITUDE
        // todo other mode
    };

    // errCode, extrenumTime, extenumAngle
    inline std::tuple<int, double, double>
    getExtrenumViaQadricApproximationAtIndex(std::shared_ptr<std::vector<double>> x,
                                             std::shared_ptr<std::vector<double>> y,
                                             const int index,
                                             int areaSize)
    {
        int errCode;
        std::vector<double> coefficients; // c, b, a

        int indexFrom = index - areaSize / 2;
        int indexTo = index + areaSize / 2;

        while (indexFrom < 0)
        {
            indexFrom++;
        }

        while (indexTo > x->size())
        {
            indexTo--;
        }

        std::tie(errCode, coefficients) =
            approximation::approxiamteQadratic(std::vector<double>(x->begin() + indexFrom, x->begin() + indexTo),
                                               std::vector<double>(y->begin() + indexFrom, y->begin() + indexTo));

        const double a = coefficients.at(2);
        const double b = coefficients.at(1);
        const double c = coefficients.at(0);

        const double xExtrenum = -b / 2.0 / a;
        const double yExtrenum = a * xExtrenum * xExtrenum + b * xExtrenum + c;

        const double xCorrection = std::abs(x->at(index) - xExtrenum);
        const double yCorrection = std::abs(y->at(index) - yExtrenum);

        return std::make_tuple(errCode, xExtrenum, yExtrenum);
    }

    struct AngleAmplitudeBase // todo rename AngleAmplitudeEssential
    {
        AngleAmplitudeBase() : m_amplitudeTime(0),
                               m_amplitudeAngle(0),
                               m_amplitudeDangle(0),
                               m_frequency(0),
                               m_period(0),
                               m_amplitudeIndexesFromInitialAngle(-1) {}

        AngleAmplitudeBase(double time, double angle, int index)
            : m_amplitudeTime(time),
              m_amplitudeAngle(angle),
              m_amplitudeIndexesFromInitialAngle(index)
        {
        }

        AngleAmplitudeBase(double time, double angle, double dangle, double frequency, double period, int index)
            : m_amplitudeTime(time),
              m_amplitudeAngle(angle),
              m_amplitudeDangle(dangle),
              m_frequency(frequency),
              m_period(period),
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

        // time, angle, dangle, frequency, amplitude index
        friend std::ostream &operator<<(std::ostream &out, const AngleAmplitudeBase &input)
        {
            out << input.m_amplitudeTime << " "
                << input.m_amplitudeAngle << " "
                << input.m_amplitudeDangle << " "
                << input.m_frequency << " "
                << input.m_period << " "
                << input.m_amplitudeIndexesFromInitialAngle << "\n";

            return out;
        }

        double m_amplitudeTime;
        double m_amplitudeAngle;
        double m_amplitudeDangle;
        double m_frequency; //< from periods
        double m_period;
        int m_amplitudeIndexesFromInitialAngle;

    private:
    };

    // bot and top angle amplitude of oscillation
    inline void calculateFrequency(std::vector<AngleAmplitudeBase>& amplitudeVector)
    {
        for (std::vector<AngleAmplitudeBase>::iterator amplitudeData = amplitudeVector.begin();
             amplitudeData != amplitudeVector.end() - 2;
             amplitudeData++)
        {
            // m_freqFromPeriod m_frequency
            amplitudeData->m_period = 2.0*((amplitudeData+1)->m_amplitudeTime - amplitudeData->m_amplitudeTime);
            amplitudeData->m_frequency = M_PI * 2.0 / amplitudeData->m_period;
        }
    }

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
                             int mode = ABS_AMPLITUDE)
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

            calculateFrequency(m_angleAmplitudeData);

            if (!isOk)
                return isOk;

            return isOk;
        }

        void sortViaTime()
        {
            std::sort(m_angleAmplitudeData.begin(), m_angleAmplitudeData.end(), [](AngleAmplitudeBase a, AngleAmplitudeBase b)
                      { return a.m_amplitudeTime < b.m_amplitudeTime; });
        }

        AngleAmplitudeBase getAmplitude(const int index) const
        {
            return m_angleAmplitudeData.at(index);
        }

        double getAmplitudeTime(const int index) const
        {
            return getAmplitude(index).m_amplitudeTime;
        }

        AngleAmplitudeBase getMaxAmplitude() const
        {
            auto it = std::max_element(m_angleAmplitudeData.begin(), m_angleAmplitudeData.end());

            if (it != m_angleAmplitudeData.end())
                return *it;

            return AngleAmplitudeBase();
        }

        AngleAmplitudeBase getMinAmplitude() const
        {
            auto it = std::min_element(m_angleAmplitudeData.begin(), m_angleAmplitudeData.end());

            if (it != m_angleAmplitudeData.end())
                return *it;

            return AngleAmplitudeBase();
        }

        size_t size() const { return m_AngleAmplitudeIndexes.size(); }
        double at(size_t index) const { return m_AngleAmplitudeIndexes.at(index); }

    private:
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
            // tmpDangle = actLinnearGaussFilter(50, 1, tmpDangle);

            for (size_t i = 1; i < tmpDangle.size(); i++)
            {
                if (tmpDangle.at(i - 1) <= 0 && tmpDangle.at(i) > 0)
                    m_AngleAmplitudeIndexes.emplace_back(i);

                if (tmpDangle.at(i - 1) >= 0 && tmpDangle.at(i) < 0)
                    m_AngleAmplitudeIndexes.emplace_back(i);
            }

            // get area arround amplitude index
            const int areaSize = 4;
            // approximate via ax^2+b*x+c -> x_extrenum = -b/(2a) y_extrenum= a(x_extrenum)^2+b(x_extrenum)+c

            m_angleAmplitudeData.reserve(m_AngleAmplitudeIndexes.size());

            for (const auto &index : m_AngleAmplitudeIndexes)
            {

                double calculatedTimeExtrenum;
                double calculatedAngleExtrenum;
                int errCode;

                std::tie(errCode, calculatedTimeExtrenum, calculatedAngleExtrenum) =
                    getExtrenumViaQadricApproximationAtIndex(m_time,
                                                             m_angle,
                                                             index,
                                                             areaSize);

                // int amplitudeIndex = m_AngleAmplitudeIndexes.at(index);
                m_angleAmplitudeData.emplace_back(
                    AngleAmplitudeBase(calculatedTimeExtrenum,
                                       (ABS_AMPLITUDE == mode) ? std::abs(calculatedAngleExtrenum) : calculatedAngleExtrenum,
                                       m_dangle->at(index),
                                       0, // will be calculated latter calculateFrequency()
                                       0, // will be calculated latter calculateFrequency()
                                       index));
            }

            return true;
        }

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
            for (const auto &ampl : input.m_angleAmplitudeData)
                out << ampl;

            return out;
        }

    public:
        std::vector<AngleAmplitudeBase> m_angleAmplitudeData;

    private:
        std::shared_ptr<std::vector<double>> m_time;
        std::shared_ptr<std::vector<double>> m_angle;
        std::shared_ptr<std::vector<double>> m_dangle;
        std::vector<int> m_AngleAmplitudeIndexes;
    };

    inline AngleAmplitudeBase getAvg(std::vector<AngleAmplitudeBase> angleAmplitudeVector)
    {
        std::vector<double> time;
        std::vector<double> angle;
        std::vector<double> dangle;
        std::vector<double> frequency;
        std::vector<double> period;
        std::vector<int> index;

        time.reserve(angleAmplitudeVector.size());
        angle.reserve(angleAmplitudeVector.size());
        dangle.reserve(angleAmplitudeVector.size());
        frequency.reserve(angleAmplitudeVector.size());
        period.reserve(angleAmplitudeVector.size());
        index.reserve(angleAmplitudeVector.size());

        for (const auto &val : angleAmplitudeVector)
        {
            time.emplace_back(val.m_amplitudeTime);
            angle.emplace_back(val.m_amplitudeAngle);
            dangle.emplace_back(val.m_amplitudeDangle);
            frequency.emplace_back(val.m_frequency);
            period.emplace_back(val.m_period);
            index.emplace_back(val.m_amplitudeIndexesFromInitialAngle);
        }

        AngleAmplitudeBase angleAmplitude(std::accumulate(time.begin(), time.end(), 0.0) / angleAmplitudeVector.size(),
                                          std::accumulate(angle.begin(), angle.end(), 0.0) / angleAmplitudeVector.size(),
                                          std::accumulate(dangle.begin(), dangle.end(), 0.0) / angleAmplitudeVector.size(),
                                          std::accumulate(frequency.begin(), frequency.end(), 0.0) / angleAmplitudeVector.size(),
                                          std::accumulate(period.begin(), period.end(), 0.0) / angleAmplitudeVector.size(),
                                          std::accumulate(index.begin(), index.end(), 0) / angleAmplitudeVector.size());

        return angleAmplitude;
    }

} // namespace amplitude
