#pragma once

#include <string>
#include <sstream>

#include <boost/filesystem.hpp>

namespace gnuplot_scripts
{

    inline std::string mz(const boost::filesystem::path &m_wtOscillationFile, const double mzNondimensionalization)
    {
        std::stringstream ss;

        ss << "plot " << m_wtOscillationFile << " using 2:($4*" << mzNondimensionalization << ") with lines"
           << std::endl;

        return ss.str();
    }

    inline std::string amplitudeLimitAmplitude(const boost::filesystem::path &wtOscillationFile,
                                               const boost::filesystem::path &angleHistroyAbsAmplitudeFile,
                                               const double limitAmplitude)
    {
        std::stringstream ss;

        ss << "plot "
           << wtOscillationFile << " using 1:2 with lines, "
           << angleHistroyAbsAmplitudeFile << " using 1:2, "
           << limitAmplitude << " lw 3 lc rgb \"red\""
           << std::endl;

        return ss.str();
    }

    namespace section_impl
    {
        inline std::string singleSection(const std::string& specificSectionFile, int sectionIndex)
        {
            std::stringstream ss;
            
                std::string graphDecoration = ("using 4:3 pt " + std::to_string(sectionIndex) + " lc " + std::to_string(sectionIndex));
                ss << "\"" + specificSectionFile + "\" " + graphDecoration;
            
            return ss.str();
        }
    }

    inline std::string amplitudeLimitAmplitude(const boost::filesystem::path &wtOscillationFile,
                                               const std::vector<boost::filesystem::path>& specificSectionFileVector)
    {
        std::stringstream ss;

        std::stringstream sectionsGnuplotFileScript;

        int sectionNo = 0;
        for (const auto &sectionFile : specificSectionFileVector)
        {
            sectionsGnuplotFileScript << ", " << section_impl::singleSection(sectionFile.string(), sectionNo);
            sectionNo++;
        }

        ss << "plot " << wtOscillationFile << " using 4:3 with linespoints"
           << (sectionsGnuplotFileScript.str().empty()
                   ? ""
                   : sectionsGnuplotFileScript.str())
           << std::endl;

        return ss.str();
    }



} // gnuplot_scripts