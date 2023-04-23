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

#include "gnuplot/data/data.h"
#include "gnuplot/picture/png_file.h"

namespace gnuplot
{
    class Wrapper
    {
    public:
        Wrapper()
        {
        }

        static std::string plotGrid()
        {
            return "set grid\n";
        }

        virtual void setPlotTitle(const std::string &titleName)
        {
            std::stringstream ss;

            ss << "set title \"" << titleName << "\"\n";

            m_title = ss.str();
        }

        virtual void setPlotAxisName(const std::string& nameX, const std::string nameY)
        {
            m_nameX = nameX;
            m_nameY = nameY;
        }

        virtual void setPngFile(png::PngFile pngFile)
        {
            m_pngFile = pngFile;
        }

        virtual bool isPngFileEnabled(){return bool(m_pngFile);}

    protected:
        std::string plotTitle() const
        {
            return m_title;
        }

        std::string getPngFileDiscryption()
        {
            if (m_pngFile)
                return m_pngFile->getCommand();

            return std::string();
        }

        std::string m_title;
        std::string m_nameX;
        std::string m_nameY;

    private:
        boost::optional<png::PngFile> m_pngFile;
        
    };

} // namespace gnuplot
