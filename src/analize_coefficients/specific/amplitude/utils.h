#pragma once

#include <vector>

#include "analize_coefficients/specific/amplitude/basic.h"
#include "analize_coefficients/specific/amplitude/basic.h"

namespace amplitude
{

    class AngleAmplitudeAnalyser : public AngleAmplitude
    {
    public:

        AngleAmplitudeAnalyser(const AngleAmplitude& angleAmplitude)
            : AngleAmplitude(angleAmplitude),
            m_sortedAmplitude(m_angleAmplitudeBase)
        {
        }

        std::vector<AngleAmplitudeBase> getSortedAmplitude() const {return m_sortedAmplitude;}

        void getMostFrequentAmplitudeValue(const int gapCount)
        {

            /*
                get amplitude
                sort via value y
                set gap of value y
                count inclusions in gap
                max inclusions in gap is the extrenum of amplitude
            */

            std::sort(m_sortedAmplitude.begin(),
                      m_sortedAmplitude.end(),
                      [](AngleAmplitudeBase a, AngleAmplitudeBase b)
                      {
                          return a.m_amplitudeAngle < b.m_amplitudeAngle;
                      });

            const double maxValue = m_angleAmplitudeBase.back().m_amplitudeAngle;
            const double minValue = m_angleAmplitudeBase.front().m_amplitudeAngle;

            int valueGapCount = 1;
            int valueCounter = 0;
            double valueGap = 1.0; // fixme describe and settle

            const int numberOfGaps = 5;
            const double gapStep = (maxValue - minValue) / numberOfGaps;

            // todo count of points in each gap
            // save count of poinst in each gap to count, avgY, avgX

            for (std::vector<AngleAmplitudeBase>::const_iterator it = m_sortedAmplitude.begin();
                 it != m_sortedAmplitude.end();
                 it++)
            {
                if (it->m_amplitudeAngle <= valueGap * valueGapCount)
                {
                    valueCounter++;
                }
                else
                {
                    m_amplitudeInGapCount.push_back(valueCounter);
                    it--;
                    valueGapCount++;
                    valueCounter = 0;
                }
            }

            std::unique(m_sortedAmplitude.begin(), m_sortedAmplitude.end());
        }

        std::vector<int> m_amplitudeInGapCount;

    private:
        std::vector<AngleAmplitudeBase> m_sortedAmplitude;
    };

} // namespace amplitude
