#pragma once

#include <tuple>
#include <vector>
#include <fstream>

#include "../oscillation/angle_history.h"
#include "../fft/fftw_impl.h"

namespace pendulum
{
    class Frequencies
    {
    public:
        Frequencies() : freq(std::vector<double>()), power(std::vector<double>())
        {
        }

        ~Frequencies() {}

    private:
        std::vector<double> freq;
        std::vector<double> power;
    };


    void remove0Harmonic(AngleHistory &angleHistory)
    {
        const double shiftAngle = angleHistory.getAngle().at(angleHistory.size()-1);
        std::cout << "shiftAngle = " << shiftAngle << "\n";

        angleHistory.moveAngle(-1.0*shiftAngle);

    }

    std::tuple<bool, Frequencies>
    getFrequencies(const AngleHistory &angleHistory)
    {
        std::cout << "getFrequencies entry\n";



        osc::fftw::Spectrum spectrum = osc::fftw::perfomFftw(angleHistory);

        std::ofstream fout("output_fft");

        for (size_t i = 0; i < spectrum.amplitude.size(); i++)
        {
            fout << i << "\t"
                 << spectrum.amplitude.at(i) << "\t"
                 << spectrum.img.at(i) << "\t"
                 << spectrum.real.at(i) << "\n";
        }

        fout.close();

        // todo calc freqs
        Frequencies freqs;

        return std::make_tuple(true, freqs);
    }
}
