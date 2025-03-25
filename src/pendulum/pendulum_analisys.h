#pragma once

#include <tuple>
#include <vector>
#include <fstream>
#include <cmath>

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
    inline void saveFile(const std::string &fileNmae, std::vector<Frequency> data)
    {
        std::cout << "saving file: " << fileNmae << " data.size() =" << data.size() << "\n";

        std::ofstream fout(fileNmae);

        for (auto k : data)
        {
            // std::cout << k.time << "\t" << k.frequency << "\n";
            fout << k.time << "\t" << k.frequency << "\n";
        }

        fout.close();
    }

    // to slow
    inline void removeOffscale(AngleHistory& angleHistory)
    {
        std::cout << "removeOffscale()\n";

        for (size_t i = angleHistory.size()-1; i > 0; --i)
        {
            if (std::abs(angleHistory.getAngle(i) - angleHistory.getAngle(i-1) > 359))
            {
                std::cout << "acts! #" << i << "\n";

                do
                {
                    // transform
                    angleHistory.setAngle(i, angleHistory.getAngle(i) - 360.0);

                    --i;

                }  while ( std::abs(angleHistory.getAngle(i) - angleHistory.getAngle(i-1)) < 100 && i > 0);
            }
        }
    }

    inline void remove0Harmonic(AngleHistory &angleHistory)
    {
        const double shiftAngle = angleHistory.getAngle().at(angleHistory.size() - 1);
        std::cout << "shiftAngle = " << shiftAngle << "\n";

        angleHistory.codomainAdd(-1.0 * shiftAngle);
    }

    namespace
    {
        void saveSpec(const osc::fftw::Spectrum &spectrum, const std::string &fileName)
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
            if (value.size() >= 2)
                return 1.0 / (value.at(1) - value.at(0));

            std::string msg = "Cant's determing discret freq!";

            std::cerr << msg << std::endl;
            throw std::runtime_error(msg);
        }

        inline size_t getMaxElementIndexFromVector(std::vector<double>::const_iterator dataFrom, std::vector<double>::const_iterator dataTo)
        {
            std::cout << "getMaxElementIndexFromVector\n";
            return std::distance(dataFrom, std::max_element(dataFrom, dataTo));
        }
    }

    inline size_t getMaxPowerFreqFromSpectrum(const osc::fftw::Spectrum &spectrum)
    {
        // return std::distance(spectrum.amplitude.begin(),
        //                      std::max_element(spectrum.amplitude.begin(), spectrum.amplitude.end()));
        return getMaxElementIndexFromVector(spectrum.amplitude.begin(), spectrum.amplitude.end());
    }

    /**
     * Get frequencies of the @angleHistory based on FFT (discretization is assumed being constant)
     * @returns true - allOk, false - error occurred
     */
    inline std::tuple<bool, std::vector<Frequency>>
    getFrequenciesViaFft(const AngleHistory &angleHistory, const uint32_t windowWidth, const uint32_t windowStep)
    {
        std::cout << "getFrequenciesViaFft entry\n";

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
            freq.frequency = maxFreqNo / discrFreq / 2.0 / PI;                                                            // main freq has max power
            freq.time = *(angleHistory.getTime().begin() + (itAngl - angleHistory.getAngle().begin()) + windowWidth / 2); // time from middle of the current window

            std::cout << "maxFreqNo = " << maxFreqNo << " freq: " << freq.frequency << " period: " << 1.0 / freq.frequency << " time: " << freq.time << "\n";

            itAngl += windowStep;

            freqs.push_back(freq);
            std::cout << "frqs.size() = " << freqs.size() << "\n";
        }

        std::cout << "getFrequencies() finished returning freqs, frqs.size() = " << freqs.size() << "\n";

        return std::make_tuple(true, freqs);
    }

    // todo rename method // no implimentation
    inline std::tuple<bool, std::vector<Frequency>>
    getFrequenciesViaSignal(const AngleHistory &angleHistory)
    {
        std::cout << "getFrequeciesViaSignal() entry\n";

        const double discrFreq = getConstDiscretisationFreq(angleHistory.getTime());
        std::cout << "discrFreq = " << discrFreq << "\n";

        std::vector<double> ang = angleHistory.getAngle(); // otherwise sigfault

        std::vector<double>::iterator itAngleStart = ang.begin();
        std::vector<double>::iterator itAngle = itAngleStart;
        std::vector<double>::iterator itAngle2 = itAngleStart;

        itAngle = std::max_element(itAngleStart, ang.end());
        // get data for itAngle

        std::vector<Frequency> freqs;

        uint counter = 1;
        /*
        while (1)
        {
            itAngle2 = std::max_element(itAngle, ang.end());
            // get data for itAngle2
            itAngle = std::max_element(itAngle2, ang.end());
            // get data for itAngle

            std::cout << "#" << counter << " " << *itAngle2 << " " << *itAngle << " ";
            std::cout <<  *(angleHistory.getTime().begin() + (itAngle2 - itAngleStart))
                      << " " << *(angleHistory.getTime().begin() + (itAngle - itAngleStart)) << "\n";

            itAngle2 += (itAngle2 - itAngleStart)/2;
            itAngle += (itAngle2 - itAngleStart)/2;
            counter++;

            if (itAngle2 + 1 == itAngle)
                break;
        }
        */

        /* size_t indexPrevMax = getMaxElementIndexFromVector(ang.begin(), ang.end());

        size_t indexNextMax = getMaxElementIndexFromVector(ang.begin() + indexPrevMax, ang.end());

        std::cout << "indexPrevMax: " << indexPrevMax << " indexNextMax:" << indexNextMax << "\n";

        Frequency freq;

        freq.frequency = 1.0 / (*(angleHistory.getTime().begin() + indexNextMax) - *(angleHistory.getTime().begin() + indexPrevMax));
        freq.time = *(angleHistory.getTime().begin() + (indexNextMax + indexPrevMax) / 2.0);

        std::cout << "indexPrevMax: " << indexPrevMax << " indexNextMax: " << indexNextMax
                  << " freq.frequency: " << freq.frequency << " freq.time: " << freq.time << "\n";

        std::vector<Frequency> freqs;
        freqs.reserve(10);

        freqs.push_back(freq);

        while (ang.end() > ang.begin() + indexNextMax)
        {

            indexNextMax = getMaxElementIndexFromVector(ang.begin() + indexPrevMax + 1, ang.end());
            freq.frequency = 1.0 / (*(ang.begin() + indexNextMax) - *(ang.begin() + indexPrevMax));
            freq.time = (*(ang.begin() + indexPrevMax) + *(ang.begin() + indexNextMax)) / 2.0;

            std::cout << "indexPrevMax: " << indexPrevMax << " indexNextMax: " << indexNextMax
                      << " search from: " << *(angleHistory.getTime().begin() + indexNextMax)
                      << " freq.frequency: " << freq.frequency << " freq.time: " << freq.time << "\n";

            // indexNextMax = indexPrevMax+1;
            indexPrevMax = indexNextMax + 1;

            freqs.push_back(freq);
            std::cout << "iter finished freqs.size() = " << freqs.size() << "\n";
        }
*/
        return std::make_tuple(true, freqs);
    }

    /*
    *   @param coefficientFreqSpred means freqs larger than coeff*discrFreq will be nglected
    */
    inline std::tuple<bool, std::vector<Frequency>>
    getFrequenciesViaDerevative(const AngleHistory &angleHistory, double coefficientFreqSpred)
    {
        std::cout << "getFrequenciesViaDerevative()\n";

        const double discrFreq = getConstDiscretisationFreq(angleHistory.getTime());

        Oscillation osc(angleHistory);

        std::vector<Frequency> freqs;

        for (int i = 0; i < osc.size() - 1; ++i)
        {

            Frequency freq;

            if (osc.getDangle(i) <= 0 && osc.getDangle(i + 1) > 0) // from top to bottom
            {

                freq.time = osc.getTime(i);

                // std::cout << "#" << i << "time curr:" << freq.time << " time prev: " << freqs.back().time << "\n";

                if (freqs.empty())
                    freqs.push_back(freq);
                else if (freq.time - freqs.back().time > coefficientFreqSpred / discrFreq)
                    freqs.push_back(freq);
            }
        }

        std::cout << "times of freqs appended\n";

        for (int i = 0; i < freqs.size() - 1; ++i)
        {
            freqs.at(i).frequency = 1.0 / (freqs.at(i + 1).time - freqs.at(i).time);
        }

        return std::make_tuple(true, freqs);
    }
}
