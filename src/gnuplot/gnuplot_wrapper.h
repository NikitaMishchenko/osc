#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <boost/optional.hpp>

#include <boost/tuple/tuple.hpp>

#include <gnuplot-iostream.h>

#include "gnuplot/data_function.h"

namespace gnuplot
{
    class Wrapper
    {
    public:
        Wrapper()
        {
        }

        virtual void setPlotTitle(const std::string &titleName)
        {
            std::stringstream ss;

            ss << "set title \"" << titleName << "\"\n";

            m_title = ss.str();
        }

    protected:
        std::string plotTitle() const
        {
            return m_title;
        }

        std::string m_title;
    };

    const double Y_RANGE_COEFF = 1.1;

    class Gnuplot1d : public Wrapper
    {
    public:
        void addDataToPlot(const DataFunction &data)
        {
            m_dataFunctionV.push_back(data);
            double xmin, xmax, ymin, ymax;
            std::tie(xmin, xmax, ymin, ymax) = data.getMaxPts();

            m_minXV.push_back(xmin);
            m_maxXV.push_back(xmax);
            m_minYV.push_back(ymin);
            m_maxYV.push_back(ymax);
        }

        void act1dVector()
        {
            Gnuplot gp;
            gp.debug_messages = true;

            std::stringstream scryptStream;

            scryptStream << plotTitle()
                         << setPlotGrid()
                         << setPlotRanges()
                         << setPlotAxies("x", "y");
            scryptStream << "plot";

            gp << scryptStream.str();
            
            // std::cout << scryptStream.str() << "\n";

            for (std::vector<DataFunction>::iterator it = m_dataFunctionV.begin(); it != m_dataFunctionV.end(); it++)
            {
                gp << gp.file1d(it->getData()) << it->getDecorations() << it->title();
                // std::cout <<  gp.file1d(it->getData()) << it->getDecorations() << m_title.str();
                
                if (m_dataFunctionV.end() != it)
                    gp << ",";
            }
            
            gp << "\n";
            
            m_scrypt = scryptStream.str();
        }

        // todo saveScript

    private:
        std::string setPlotRanges() const
        {
            std::stringstream ss;

            const double ymax = *std::max_element(m_maxYV.begin(), m_maxYV.end());
            const double ymin = *std::min_element(m_minYV.begin(), m_minYV.end());
            const double xmax = *std::max_element(m_maxXV.begin(), m_maxXV.end());
            const double xmin = *std::min_element(m_minXV.begin(), m_minXV.end());

            ss << "set xrange [" << xmin << ":" << xmax << "]\n";
            ss << "set yrange [" << ymin * Y_RANGE_COEFF << ":" << ymax * Y_RANGE_COEFF << "]\n";

            std::cout << ss.str();

            return ss.str();
        }

        std::string setPlotAxies(const std::string &xName, const std::string &yName) const
        {
            std::stringstream ss;

            ss << "set xlabel \'" << xName << "\'\n";
            ss << "set ylabel \'" << yName << "\'\n";

            return ss.str();
        }

        std::string setPlotGrid() const
        {
            return "set grid\n";
        }

        std::vector<DataFunction> m_dataFunctionV; // todo std::variant 
        std::vector<double> m_minYV;
        std::vector<double> m_maxYV;
        std::vector<double> m_minXV;
        std::vector<double> m_maxXV;

        std::string m_scrypt;
    };

} // namespace gnuplot
