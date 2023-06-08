#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <tuple>

#include <boost/optional.hpp>

#include "gnuplot/data/data_base.h"

namespace gnuplot
{
    struct ConfigGnuplot1D
    {
        int rangeXMode;
        int rangeYmode;
    };

    class DataFunction : public DataBase
    {
    public:
        DataFunction(const std::vector<double> &argument,
                     const std::vector<double> &function)
        {
            if (argument.size() != function.size())
                throw std::out_of_range(std::string("DataFunction argument and function sizes was not matched").c_str());

            for (size_t i = 0; i < argument.size(); i++)
                m_xyPts.push_back(std::make_pair(argument.at(i), function.at(i)));

            m_ymax = std::max_element(m_xyPts.begin(), m_xyPts.end(),
                                      [](const std::pair<double, double> &aPts,
                                         const std::pair<double, double> &bPts)
                                      { return aPts.second < bPts.second; })
                         ->second;
            m_ymin = std::min_element(m_xyPts.begin(), m_xyPts.end(),
                                      [](const std::pair<double, double> aPts,
                                         const std::pair<double, double> bPts)
                                      { return (aPts.second < bPts.second); })
                         ->second;

            m_xmax = std::max_element(m_xyPts.begin(), m_xyPts.end(),
                                      [](const std::pair<double, double> &aPts,
                                         const std::pair<double, double> &bPts)
                                      { return aPts.first < bPts.first; })
                         ->first;
            m_xmin = std::min_element(m_xyPts.begin(), m_xyPts.end(),
                                      [](const std::pair<double, double> aPts,
                                         const std::pair<double, double> bPts)
                                      { return (aPts.first < bPts.first); })
                         ->first;
        }

        std::vector<std::pair<double, double>> getData() const
        {
            return m_xyPts;
        }

    protected:

        std::vector<std::pair<double, double>> m_xyPts;
    };

} // namespace gnuplot
