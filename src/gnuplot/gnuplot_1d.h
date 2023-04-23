#pragma once

#include <string>
#include <vector>

#include <gnuplot-iostream.h>

#include "gnuplot/data/data.h"
#include "gnuplot/gnuplot_wrapper.h"

namespace gnuplot
{
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

        void addDataToPlot(const DataGnuplotFunction& data)
        {
            m_dataGnuplotFunctionV.push_back(data);
        }

        void act1dVector()
        {
            Gnuplot gp;
            gp.debug_messages = true;

            std::stringstream scryptStream;

            scryptStream << getPngFileDiscryption()
                         << plotTitle()
                         << plotGrid()
                         << plotRanges()
                         << plotAxiesName(m_nameX, m_nameY);
            scryptStream << "plot";

            gp << scryptStream.str();
            // std::cout << scryptStream.str() << "\n";

            for (std::vector<DataFunction>::iterator it = m_dataFunctionV.begin(); it != m_dataFunctionV.end(); it++)
            {
                gp << gp.file1d(it->getData()) << it->getFunctionDecorations();
                // std::cout <<  gp.file1d(it->getData()) << it->getDecorations() << m_title.str();
                
                if (m_dataFunctionV.end() != it && !m_dataGnuplotFunctionV.empty())
                    gp << ",";
            }
            
            for (std::vector<DataGnuplotFunction>::iterator it = m_dataGnuplotFunctionV.begin(); it != m_dataGnuplotFunctionV.end(); it++)
            {
                gp << it->dataFunction();
            }

            gp << "\n";
            
            // std::cout << scryptStream.str() << "\n";
            
            m_scrypt = scryptStream.str();
        }

        // todo saveScript

    private:

        std::string plotRanges() const
        {
            std::stringstream ss;

            double ymax = 0;
            if (!m_maxXV.empty())
                ymax = *std::max_element(m_maxYV.begin(), m_maxYV.end());
            
            double ymin = 0;
            if (!m_minYV.empty())
                ymin = *std::min_element(m_minYV.begin(), m_minYV.end());
            
            double xmax = 0;
            if (!m_maxXV.empty())
                xmax = *std::max_element(m_maxXV.begin(), m_maxXV.end());
            
            double xmin = 0;
            if (!m_minXV.empty())
                xmin = *std::min_element(m_minXV.begin(), m_minXV.end());

            if (ymin == ymax == 0)
                ss << "set xrange [" << xmin << ":" << xmax << "]\n";
            
            if (ymax == ymin == 0)
                ss << "set yrange [" << ymin * Y_RANGE_COEFF << ":" << ymax * Y_RANGE_COEFF << "]\n";

            std::cout << ss.str();

            return ss.str();
        }

        std::string plotAxiesName(const std::string &xName, const std::string &yName) const
        {
            std::stringstream ss;

            ss << "set xlabel \'" << xName << "\'\n";
            ss << "set ylabel \'" << yName << "\'\n";

            return ss.str();
        }

        std::vector<DataFunction> m_dataFunctionV; // todo std::variant 
        std::vector<DataGnuplotFunction> m_dataGnuplotFunctionV;
        std::vector<double> m_minYV;
        std::vector<double> m_maxYV;
        std::vector<double> m_minXV;
        std::vector<double> m_maxXV;

        std::string m_scrypt;
    };

} // gnuplot
