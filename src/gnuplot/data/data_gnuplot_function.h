#pragma once

#include <string>

#include "gnuplot/data/data_base.h"

namespace gnuplot
{

    class DataGnuplotFunction : public DataBase
    {
    public:
        DataGnuplotFunction(const std::string &function) : m_function(function)
        {
        }

        void setFunction(const std::string &function)
        {
            m_function = function;
        }

        std::string dataFunction() const
        {
            return m_function;
        }

        void setRanges(double xmin, double xmax, double ymin, double ymax)
        {
            m_xmin = xmin;
            m_xmax = xmax;
            m_ymin = ymin;
            m_ymax = ymax;
        }

    private:
        std::string m_function;
    };

} // namespace gnuplot
