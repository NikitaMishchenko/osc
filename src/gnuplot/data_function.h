#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <tuple>

#include <boost/optional.hpp>

namespace gnuplot
{
    enum LineType
    {
        LINES,
        LINES_POINTS,
        POINTS
    };

    enum LineColor
    {
        BLACK  = -1,
        GREY   = 0,
        GREEN  = 2,
        BLUE   = 3,
        ORANGE = 4,
        YELLOW = 5,
        NAVY   = 6,
        RED    = 7
    };

    struct ConfigGnuplot1D
    {
        int rangeXMode;
        int rangeYmode;
    };

    class DataFunction
    {
    public:
        DataFunction(const std::vector<double> &argument,
                     const std::vector<double> &function)
        {
            if (argument.size() != function.size())
                throw std::exception();

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

        std::string getDecorations() const
        {
            std::stringstream ss;

            ss << lineType() << lineColor();

            return ss.str();
        }

        void setLineType(const int lineType)
        {
            m_lineType = lineType;
        }

        void setLineColor(const int lineColor)
        {
            m_lineColor = lineColor;
        }

        void setTitle(const std::string& title)
        {
            m_title = title;
        }

        std::string title() const
        {
            if (!m_title)
                return std::string();

            return m_title.get();
        }

        std::tuple<double, double, double, double> getMaxPts() const
        {
            return std::make_tuple(m_xmin, m_xmax, m_ymin, m_ymax);
        }

    protected:
        std::string lineType() const
        {

            if (m_lineType)
                switch (m_lineType.get())
                {
                case LINES:
                    return " with lines ";

                case LINES_POINTS:
                    return " with linespoints ";

                case POINTS:
                    return " with points ";

                default:
                    return std::string();
                }

            return std::string();
        }

        std::string lineColor() const
        {
            if (!m_lineColor)
                return " linecolor -1";

            std::stringstream ss;
            ss << " linecolor " << m_lineColor.get() << " ";

            return ss.str();
        }

        std::vector<std::pair<double, double>> m_xyPts;
        double m_xmin;
        double m_xmax;
        double m_ymin;
        double m_ymax;
        boost::optional<int> m_lineType;
        boost::optional<int> m_lineColor;
        boost::optional<std::string> m_title;
    };

} // namespace gnuplot