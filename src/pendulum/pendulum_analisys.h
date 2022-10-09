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

        /// fixme move to helpers i/o    
        void saveFile(const std::string& fileNmae, std::vector<Frequency> data)
        {
            std::cout << "saving file: " << fileNmae << " data.size() =" << data.size() << "\n";

            std::ofstream fout(fileNmae);

            for (auto k : data)
            {
                //std::cout << k.time << "\t" << k.frequency << "\n";
                fout << k.time << "\t" << k.frequency << "\n";
            }
                        
            fout.close();
        }

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

        inline double getConstDiscretisationFreq(const std::vector<double> value)
        {
            if (value.size() >=2 )
                return 1.0/(value.at(1) - value.at(0));

            std::string msg = "Cant's determing discret freq!";

            std::cerr << msg << std::endl;
            throw std::runtime_error(msg);           
        }
    }

    /**
     * Получение частоты колебаний под
     * @returns true - allOk, false - error occurred
     */
    std::tuple<bool, std::vector<Frequency>>
    getFrequencies(const AngleHistory& angleHistory, const uint32_t windowWidth, const uint32_t windowStep)
    {
        std::cout << "getFrequencies entry\n";

        const double discrFreq = getConstDiscretisationFreq(angleHistory.getTime());
        std::cout << "discrFreq = " << discrFreq << "\n";

        std::vector<Frequency> freqs;
        freqs.reserve(10);
        // std::cout << "frqs.size() = " << freqs.size() << "\n";  

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

            int64_t maxFreqNo = getMaxPowerFreqFromSpectrum(spectrum);
            freq.frequency = maxFreqNo / discrFreq / 2.0 / PI; // main freq has max power
            freq.time = *(angleHistory.getTime().begin() + (itAngl - angleHistory.getAngle().begin()) + windowWidth / 2); // time from middle of the current window

            std::cout << "maxFreqNo = " << maxFreqNo << " freq: " << freq.frequency <<  " period: " << 1.0/freq.frequency  << " time: " << freq.time << "\n";

            itAngl += windowStep;

            freqs.push_back(freq);
            std::cout << "frqs.size() = " << freqs.size() << "\n";
        }

        std::cout << "getFrequencies() finished returning freqs, frqs.size() = " << freqs.size() << "\n";

        return std::make_tuple(true, freqs);
    }
}
