#pragma once

#include <string>
#include <sstream>

#include <boost/filesystem.hpp>

namespace
{
    inline std::string commonDescription(const std::string &title = std::string(),
                                         const std::string &xLable = std::string(),
                                         const std::string &yLable = std::string())
    {
        std::stringstream ss;

        ss << "set grid" << std::endl;

        if (!title.empty())
            ss << "set title \"" << title << "\"" << std::endl;

        if (!xLable.empty())
            ss << "set xlabel \"" << xLable << "\"" << std::endl;

        if (!yLable.empty())
            ss << "set ylabel \"" << yLable << "\"" << std::endl;

        ss << std::endl;

        return ss.str();
    }
}

namespace gnuplot_scripts
{

    inline std::string mz(const boost::filesystem::path &m_wtOscillationFile, const double mzNondimensionalization)
    {
        std::stringstream ss;

        const std::string title = "\"m_z({/Symbol a})\"";

        ss << commonDescription("m_z({/Symbol a})", "{/Symbol a}", "m_z");

        ss << "plot " << m_wtOscillationFile << " using 2:($4*" << mzNondimensionalization << ") with lines title " << title
           << std::endl;

        return ss.str();
    }

    inline std::string amplitudeLimitAmplitude(const boost::filesystem::path &wtOscillationFile,
                                               const boost::filesystem::path &angleHistroyAbsAmplitudeFile,
                                               const double limitAmplitude,
                                               const std::string &coreName)
    {
        std::stringstream ss;

        const std::string titleAngleHistory = "\"{/Symbol a}(t)\"";
        const std::string titleAbsAmplitude = "\"{/Symbol q}(t)\"";
        const std::string titleLimitAmplitude = "\"limit amplitude\"";

        ss << commonDescription(std::string("angle history " + coreName), "t", "{/Symbol a}(t)");

        ss << "plot "
           << wtOscillationFile << " using 1:2 with lines title " << titleAngleHistory << ", "
           << angleHistroyAbsAmplitudeFile << " using 1:2 title " << titleAbsAmplitude << ", "
           << limitAmplitude << " lw 3 lc rgb \"red\" title " << titleLimitAmplitude << " "
           << std::endl;

        return ss.str();
    }

    namespace section_impl
    {
        inline std::string singleSection(const std::string &specificSectionFile, int sectionIndex)
        {
            std::stringstream ss;

            std::string graphDecoration = "using 4:3 pt " + std::to_string(sectionIndex) + " lc " + std::to_string(sectionIndex) + " notitle";
            ss << "\"" + specificSectionFile + "\" " + graphDecoration;

            return ss.str();
        }
    }

    inline std::string amplitudeLimitAmplitude(const boost::filesystem::path &wtOscillationFile,
                                               const std::vector<boost::filesystem::path> &specificSectionFileVector,
                                               const std::string &coreName)
    {
        std::stringstream ss;

        std::stringstream sectionsGnuplotFileScript;
        {
            int sectionNo = 0;
            for (const auto &sectionFile : specificSectionFileVector)
            {
                sectionsGnuplotFileScript << ", " << section_impl::singleSection(sectionFile.string(), sectionNo);
                sectionNo++;
            }
        }

        ss << commonDescription(std::string("amitude " + coreName + "5 degree step"), "{/Symbol w}_n_o_n_d", "{/Symbol q}({/Symbol w}_n_o_n_d)");

        ss << "plot " << wtOscillationFile << " using 4:3 with linespoints"
           << (sectionsGnuplotFileScript.str().empty()
                   ? ""
                   : sectionsGnuplotFileScript.str())
           << std::endl;

        return ss.str();
    }

    inline std::string amplitudeSummary(const boost::filesystem::path &descriptionFileName)
    {
        std::stringstream ss;
        const std::string titleAmplitudeSummary = "\"{/Symbol q}({/Symbol w}_n_o_n_d)\"";

        ss << "set yrange [0:50]" << std::endl;
        ss << "set xrange [0.01:0.018]" << std::endl;

        ss << commonDescription("summary {/Symbol q}({/Symbol w}_n_o_n_d)",
                                "{/Symbol w}_n_o_n_d",
                                "{/Symbol q}");

        ss << "plot " << descriptionFileName << " using 4:2 lw 10 notitle "; // << titleAmplitudeSummary << std::endl;

        return ss.str();
    }

} // gnuplot_scripts