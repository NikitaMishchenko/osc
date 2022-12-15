#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <iterator>
#include <algorithm>

#include "core/math.h"

#include "angle_history.h"

namespace oscillation_helpers
{

    enum FileType
    {
        DEFAULT,
        TIME_ANGLE,
        TIME_ANGLE_DANGLE,
        TIME_ANGLE_DANGLE_DDANGLE
    };

    // todo rewrite
    /**
     *   когда есть больше 2 точек в строке
     */
    inline bool isOscillationFile(const std::string &fileName)
    {
        std::ifstream fin(fileName);

        std::string str1;

        std::getline(fin, str1);

        std::cout << "get first string: " << str1 << "\n";

        int findOne = 0;

        size_t pos = 0;
        while (1)
        {
            pos = str1.find('.', pos);
            pos++;

            if (std::string::npos == pos || pos == 0)
            {
                break;
            }

            findOne++;

            if (findOne > 2)
            {
                std::cout << "probably osillation file\n";
                return true;
            }
        }

        fin.close();

        std::cout << "probably raw file\n";

        return false;
    }

    /*FileType typeOfOscillationFile(const std::string& fileName)
    {
        std::vector<std::string> tokens;
        std::istringstream   mySstream(fileName);
        std::string         temp;

        const char delim = ' ';

        while (getline(mySstream, temp, delim))
        {
            tokens.push_back( temp );
        }

        //return tokens;
        return DEFAULT;
    }*/

} // namespace oscillation

enum LOG_MODE
{
    LOG_ON,
    LOG_OFF
};

class Oscillation : public AngleHistory
{
public:
    Oscillation() : AngleHistory(), dangle(), ddangle()
    {
    }

    Oscillation(const AngleHistory &angleHistory);
    Oscillation(const std::string &file_name);

    virtual void recalculate();

    virtual ~Oscillation();

    Oscillation(const Oscillation &d);
    Oscillation &operator=(const Oscillation &d);

    virtual std::vector<double>::const_iterator timeBegin() const
    {
        return m_domain.begin();
    }

    virtual std::vector<double>::const_iterator timeEnd() const
    {
        return m_domain.end();
    }

    virtual std::vector<double>::const_iterator angleBegin() const
    {
        return m_codomain.begin();
    }

    virtual std::vector<double>::const_iterator angleEnd() const
    {
        return m_codomain.end();
    }

    virtual std::vector<double>::const_iterator dangleBegin() const
    {
        return dangle.begin();
    }

    virtual std::vector<double>::const_iterator dangleEnd() const
    {
        return dangle.end();
    }

    virtual std::vector<double>::const_iterator ddangleBegin() const
    {
        return ddangle.begin();
    }

    virtual std::vector<double>::const_iterator ddangleEnd() const
    {
        return ddangle.end();
    }

    virtual double getDangle(const size_t index) const
    {
        return dangle.at(index);
    }

    virtual double getDdangle(const size_t index) const
    {
        return ddangle.at(index);
    }

    // todo refactor
    virtual void insertAllFieldsFromTo(const Oscillation A, size_t from, size_t to);

    // BASIC STL like
    virtual void reserve(const size_t s) override;
    virtual void push_back(const double &t, const double &a, const double &da, const double &dda);
    virtual void clear() override;

    virtual const double getDangle(int i) const { return dangle.at(i); }
    virtual const std::vector<double> getDangle() const { return dangle; }

    virtual double getDdangle(int i) const { return ddangle.at(i); }
    virtual const std::vector<double> getDdangle() const { return ddangle; }

    // IO
    // IO
    friend std::ostream &operator<<(std::ostream &outSource, const Oscillation &D)
    {
        for (size_t i = 0; i < D.size(); i++)
        {
            outSource << D.m_domain.at(i) << "\t"
                      << D.m_codomain.at(i) << "\t"
                      << D.dangle.at(i) << "\t"
                      << D.ddangle.at(i) << "\n";
        }

        return outSource;
    }

    friend std::istream &operator>>(std::istream &inSource, Oscillation &D)
    {
        double d0_buff, d1_buff, d2_buff, d3_buff;

        while (!inSource.eof())
        {
            inSource >> d0_buff >> d1_buff >> d2_buff >> d3_buff;

            D.push_back(d0_buff, d1_buff, d2_buff, d3_buff);
        }

        return inSource;
    }

    virtual void print() const override;

    // todo rename
    virtual void write(const std::string &file_name) const override;

    /*
     *   Load Oscillation file saved in 'write"-form
     *   TODO refactor
     */
    virtual bool loadFile(const std::string &fileName,
                          oscillation_helpers::FileType mode = oscillation_helpers::DEFAULT);

    /// SPECIAL
    // use vector helpers
    void scaleDAngle(const double &factor);
    void scaleDDAngle(const double &factor);

    /// OTHER
    virtual void info() const override;

private:
    std::vector<double> dangle;
    std::vector<double> ddangle;
};
