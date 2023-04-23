#pragma once

#include <string>
#include <sstream>

#include <boost/optional.hpp>

#include <cstdint>

namespace gnuplot::png
{
    struct PngProperties
    {
        PngProperties(const int resWidth, const int resHeight) : m_resWidth(resWidth), m_resHeight(resHeight)
        {
        }

        int m_resWidth;
        int m_resHeight;
        // todo background, border, X, Y, plots color
    };

    class PngFile
    {
    public:
        PngFile() : m_fileName("picture")
        {}

        PngFile(const std::string &fileName) : m_fileName(fileName)
        {}

        void setFileName(const std::string &fileName)
        {
            m_fileName = fileName;
        }

        void setPngProperties(const PngProperties& pngProperties)
        {
            m_pngProperties = pngProperties;
        }

        std::string getCommand() const
        {
            std::stringstream ss;

            ss << "set terminal png medium ";
            
            if (m_pngProperties)
                ss << "size " << m_pngProperties->m_resWidth << ", " << m_pngProperties->m_resHeight << "\n";
            else 
                ss << "\n";    

            ss << "set output \"" << m_fileName << ".png\"\n";    

            /*"set terminal png medium size 640,480 \
                      xffffff x000000 x404040 \
                      xff0000 xffa500 x66cdaa xcdb5cd \
                      xadd8e6 x0000ff xdda0dd x9500d3"*/

            return ss.str();
        }

    private:
        std::string m_fileName;

        boost::optional<PngProperties> m_pngProperties;
    };

} // gnuplot::png
