#pragma once

#include <tuple>
#include <vector>
#include <fstream>

#include "../oscillation/angle_history.h"
#include "../fft/fftw_impl.h"

namespace pendulum
{
    class Frequency
    {
    public:
        Frequency() : time(0), frequency(0)
        {
        }

        ~Frequency() {}

        double time;
        double frequency;
    };

    void remove0Harmonic(AngleHistory &angleHistory)
    {
        const double shiftAngle = angleHistory.getAngle().at(angleHistory.size() - 1);
        std::cout << "shiftAngle = " << shiftAngle << "\n";

        angleHistory.moveAngle(-1.0 * shiftAngle);
    }

    inline size_t getMaxPowerFreqFromSpectrum(const osc::fftw::Spectrum &spectrum)
    {
        return std::distance(spectrum.amplitude.begin(),
                             std::max_element(spectrum.amplitude.begin(), spectrum.amplitude.end()));
    }

    namespace
    {
        void saveSpec(const osc::fftw::Spectrum &spectrum, const std::string& fileName)
        {
            std::ofstream fout(fileName);

            for (size_t i = 0; i < spectrum.amplitude.size(); i++)
            {
                fout << i << "\t"
                     << spectrum.amplitude.at(i) << "\t"
                     << spectrum.img.at(i) << "\t"
                     << spectrum.real.at(i) << "\n";
            }

            fout.close();
        }
    }

    /**
     * Получение частоты колебаний под
     * @returns true - allOk, false - error occurred
     */
    std::tuple<bool, std::vector<Frequency>>
    getFrequencies(const AngleHistory& angleHistory, const size_t windowWidth, const size_t windowStep)
    {
        std::cout << "getFrequencies entry\n";

        std::vector<Frequency> freqs;

        int i = 0;
        for (std::vector<double>::const_iterator itAngl = angleHistory.getAngle().begin();;)
        {
            if ((itAngl + windowWidth) >= angleHistory.getAngle().end())
                break;

            osc::fftw::Spectrum spectrum = osc::fftw::perfomFftw(itAngl, itAngl + windowWidth);

            std::string fileName = "spec_" + std::to_string(i);
            saveSpec(spectrum, fileName);
            i++;

            Frequency freq;

            // todo coefficient to getFreq
            freq.frequency = getMaxPowerFreqFromSpectrum(spectrum);              // main freq has max power
            freq.time = *(angleHistory.getTime().begin() + (itAngl - angleHistory.getAngle().begin()) + windowWidth / 2); // time from middle of the window

            std::cout << "freq: " << freq.frequency << " time: " << freq.time << "\n";

            itAngl += windowStep;
        }

        // todo calc freqs

        return std::make_tuple(true, freqs);
    }
}
