#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "core/function.h"

#include "oscillation/angle_history.h"

AngleHistory::AngleHistory(const std::vector<double> &timeIn,
                           const std::vector<double> &angleIn)
    : Function(timeIn, angleIn)
{
    calculateTimeStep();
}

AngleHistory::AngleHistory(const std::string &file_name)
{
    this->loadRaw(file_name);

    calculateTimeStep();
}

void AngleHistory::clear()
{
    Function::clear();
    m_timeStep = 0;
}

void AngleHistory::print() const
{
    std::cout << *this;
}

void AngleHistory::info() const
{
    std::cout << "AngleHistory object \n"
              << "time size: " << m_domain.size() << "\n"
              << "angle size: " << m_codomain.size() << "\n"
              << "timeStep: " << m_timeStep << "\n";
}

bool AngleHistory::loadRaw(const std::string &file_name)
{
    std::ifstream fin(file_name);
    std::cout << "trying open file: " << file_name << std::endl;

    if (fin.is_open())
    {
        fin >> *this;

        std::cout << "file " << file_name << " loaded! Closing\n";
        fin.close();

        calculateTimeStep();

        return true;
    }
    else
    {
        std::cerr << "File " << file_name << " was not found. Loading failed\n";
    }

    fin.close();

    return false;
}

void AngleHistory::write(const std::string &fileName) const
{
    std::ofstream fout(fileName);

    fout << *this;

    fout.close();
}

void AngleHistory::calculateTimeStep()
{
    if (size())
        m_timeStep = ((size() >= 2) ? (m_domain.at(1) - m_domain.at(0)) : 0.0);
    else
        m_timeStep = 0.0;
}

double AngleHistory::indexToTime(const size_t index) const
{
    if (index < size())
        return m_domain.at(index);

    return m_domain.at(size() - 1);
}

size_t AngleHistory::timeToIndex(const double timeValue) const
{
    // time assumed started at 0.0
    if (!size() && !m_timeStep)
        return 0;

    return timeValue / m_timeStep;
}

std::vector<double>::const_iterator AngleHistory::timeIterator(const double timeValue) const
{
    return m_domain.cbegin() + timeToIndex(timeValue);
}
