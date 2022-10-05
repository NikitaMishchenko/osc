#pragma once

#include <tuple>
#include <vector>

#include "../oscillation/angle_history.h"
#include "../fft/fftw_impl.h"

namespace pendulum
{
    class Frequencies
    {
    public:
        Frequencies() : freq(std::vector<double>()), power(std::vector<double>())
        {}

        ~Frequencies(){}
    
    private:
        std::vector<double> freq;
        std::vector<double> power;
    };

    std::tuple<bool, Frequencies>
        getFrequencies(const AngleHistory& angleHistory)
    {
        Frequencies freqs;
        
        osc::fftw::Spectrum spectrum = osc::fftw::perfomFftw(angleHistory);

        return std::make_tuple(true, freqs);
    }
}
