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
    {}

    AngleHistory(const std::vector<double> &timeIn, const std::vector<double> &angleIn);

    AngleHistory(const std::string &file_name);

    virtual ~AngleHistory(){}

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

    // STL-like
    virtual void clear() override;

    virtual void info() const;
    virtual void print() const;
    virtual bool loadRaw(const std::string &file_name);
    virtual void write(const std::string &fileName) const;

    friend std::ostream &operator<<(std::ostream &out, const AngleHistory &D);
    friend std::istream &operator>>(std::ifstream &inSource, AngleHistory& Data);

protected:
    void calculateTimeStep(); 
    double indexToTime(const size_t index) const;
    size_t timeToIndex(const double timeValue) const;

    /*std::vector<double>::iterator timeIterator(const double timeValue) const
    {
        return (m_domain.begin() + timeToIndex(timeValue));
    }*/

    std::vector<double>::const_iterator timeIterator(const double timeValue) const;

    double m_timeStep;
};
