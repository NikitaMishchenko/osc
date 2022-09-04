#pragma once

#include <tuple>
#include <vector>

#include "../oscillation/oscillation_basic.h"

namespace pendulum
{
    class Frequencies
    {
        std::vector<double> row;
        std::vector<double> corrected;

    };

    std::tuple<bool, Frequencies>
        getFrequencies(const AngleHistory& angleHistory)
    {
        Frequencies freqs;



    }
}
