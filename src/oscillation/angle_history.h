#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "core/function.h"

/*
 * Assumed constant timeStep
 */
class AngleHistory : public Function // todo rename it's better be like TwoVectors // on the higher lvl make it angle and time
{
public:
    AngleHistory() : Function(), m_timeStep(0.0)
    {
    }

    AngleHistory(const std::vector<double> &timeIn,
                 const std::vector<double> &angleIn)
        : Function(timeIn, angleIn)
    {
        calculateTimeStep();
    }

    AngleHistory(const std::string &file_name)
    {
        this->loadRaw(file_name);

        calculateTimeStep();
    }

    virtual ~AngleHistory()
    {}

    /*
    //copy
    AngleHistory(const AngleHistory& d) : m_domain(d.m_domain), m_codomain(d.m_codomain)
    {
        //std::cout << "AngleHistory copy constructor\n";
    }

    AngleHistory& operator= (const AngleHistory& d)
    {
        m_time = d.m_time;
        m_angle = d.m_angle;

        return *this;
    }
    */

    double getTimeStep() const
    {
        return m_timeStep;
    }

    std::vector<double> getAngle() const
    {
        return m_codomain;
    }

    double getAngle(int index) const
    {
        return m_codomain.at(index);
    }

    void setAngle(size_t index, const double value)
    {
        m_codomain.at(index) = value;
    }

    // todo remove? it's unsafe
    void setAngle(const std::vector<double> &newAngle)
    {
        m_codomain = newAngle;
    }

    std::vector<double> getTime() const
    {
        return m_domain;
    }

    double getTime(int index) const
    {
        return m_domain.at(index);
    }

    void setTime(size_t index, const double value)
    {
        m_domain.at(index) = value;
    }

    void setTime(const std::vector<double> &newTime)
    {
        m_domain = newTime;
    }

    virtual void print()
    {
        std::cout << *this;
    }

    /*
     * TODO make overload for operator>>
     */
    friend std::ostream &operator<<(std::ostream &out, const AngleHistory &D)
    {
        for (size_t i = 0; i < D.size(); i++)
        {
            out << D.m_domain.at(i) << "\t"
                << D.m_codomain.at(i) << "\t"
                << "\n";
        }

        return out;
    }

    virtual void info() const
    {
        std::cout << "AngleHistory object \n"
                  << "time size: " << m_domain.size() << "\n"
                  << "angle size: " << m_codomain.size() << "\n"
                  << "timeStep: " << m_timeStep << "\n";
    }

    virtual bool loadRaw(const std::string &file_name)
    {
        std::ifstream fin(file_name);
        std::cout << "trying open file: " << file_name << std::endl;

        if (fin.is_open())
        {
            while (!fin.eof())
            {
                double b_angle, b_time;

                fin >> b_time >> b_angle;

                this->push_back(b_time, b_angle);
            }

            std::cout << "file " << file_name << " loaded! Closing\n";
            fin.close();
            return true;
        }
        else
        {
            std::cerr << "File " << file_name << " was not found. Loading failed\n";
        }

        fin.close();
        return false;
    }

    virtual void write(const std::string &fileName) const
    {
        std::ofstream fout(fileName);

        fout << *this;

        fout.close();
    }

protected:
    void calculateTimeStep()
    {
        if (size())
            m_timeStep = ((size() >= 2) ? (m_domain.at(1) - m_domain.at(0)) : 0.0);
        else
            m_timeStep = 0.0;    
    }

    double indexToTime(const size_t index) const
    {
        if(index < size())
            return m_domain.at(index);

        return m_domain.at(size()-1);    
    }

    size_t timeToIndex(const double timeValue) const
    {
        // time assumed started at 0.0
        if (!size() && !m_timeStep)
            return 0;    

        return timeValue/m_timeStep;
    }

    /*std::vector<double>::iterator timeIterator(const double timeValue) const
    {
        return (m_domain.begin() + timeToIndex(timeValue));
    }*/

    std::vector<double>::const_iterator timeIterator(const double timeValue) const
    {
        return m_domain.cbegin() + timeToIndex(timeValue);
    }

    double m_timeStep;
};
