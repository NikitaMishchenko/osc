#pragma once

#include <vector>

namespace
{
    // todo move to function helpers // todo use GNU SL
    double derevativeOrd1N2(const std::vector<double> &func,
                            const double &h,
                            const size_t index)
    {

        double d = 0.0;
        // left border
        if (0 == index)
        {
            d = (-3.0 * func.at(0) + 4.0 * func.at(1) - func.at(2)) / 2.0 / h;

            return d;
        }

        // center
        if (index > 0 && index < func.size() - 1)
        {
            d = (func.at(index + 1) - func.at(index - 1)) / 2.0 / h; // ~h^2/6(f''')

            return d;
        }
        // if (index == func.size()-1)
        //     return (func.at())
        //  right border
        return 0;
    }

    double derevativeOrd2N2(const std::vector<double> &func, const double &h, const size_t index)
    {
        if (index > 0 && index < func.size() - 2)
            return (func.at(index + 2) - 2.0 * func.at(index + 1) + func.at(index)) / h / h;

        return 0;
    }
} // namespace

std::vector<double> derevative1Order(const std::vector<double>& input, const double argStep)
{
    std::vector<double> result;

    result.reserve(input.size());

    for(size_t i = 0; i < input.size(); ++i)
        result.emplace_back(derevativeOrd1N2(input, argStep, i));

    return result;    
}
