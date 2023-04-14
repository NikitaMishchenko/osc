#pragma once

#include <string>
#include <sstream>

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
        BLACK = -1,
        GREY = 0,
        GREEN = 2,
        BLUE = 3,
        ORANGE = 4,
        YELLOW = 5,
        NAVY = 6,
        RED = 7
    };

    class DataBase
    {
    public:
        DataBase(){};

        virtual ~DataBase() {}

        virtual void setLineType(const int lineType)
        {
            m_lineType = lineType;
        }

        virtual void setLineColor(const int lineColor)
        {
            m_lineColor = lineColor;
        }

        virtual void setTitle(const std::string &title)
        {
            m_title = title;
        }

        std::string getFunctionDecorations() const
        {
            std::stringstream ss;

            ss << lineType() << lineColor() << title();

            return ss.str();
        }

        std::tuple<double, double, double, double> getMaxPts() const
        {
            return std::make_tuple(m_xmin, m_xmax, m_ymin, m_ymax);
        }

        virtual std::string title() const
        {
            if (!m_title)
                return std::string();

            std::stringstream ss;

            ss << " title \'" << m_title.get() << "\' ";

            return ss.str();
        }

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

    protected:
        double m_xmin;
        double m_xmax;
        double m_ymin;
        double m_ymax;

        boost::optional<int> m_lineType;
        boost::optional<int> m_lineColor;
        boost::optional<std::string> m_title;
    };

} // namespace gnuplot
