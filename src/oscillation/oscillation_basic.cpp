#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <iterator>
#include <algorithm>

#include "core/math.h"
#include "angle_history.h"

#include "oscillation_basic.h"

Oscillation::Oscillation(const AngleHistory &angleHistory) : AngleHistory(angleHistory)
{
    std::cout << "Oscillation(AngleHistory) constructor()\n";

    this->recalculate();
}

Oscillation::Oscillation(const std::string &file_name) : AngleHistory(file_name)
{
    std::cout << "Oscillation( " << file_name << ") constructor\n";
    this->recalculate();
}

void Oscillation::recalculate()
{
    if (!AngleHistory::empty())
    {
        dangle.reserve(this->size());
        ddangle.reserve(this->size());

        const double h = getTimeStep();

        /// dangle calc
        dangle = derevative1Order(m_codomain, h);

        // todo filter ?

        /// ddangle calc
        dangle = derevative1Order(dangle, h);

        // todo filter ?
    }
    else
    {
        std::cout << "empty AngleHistory -> empty Oscillation \n";
        dangle.reserve(0);
        ddangle.reserve(0);
    }
}

Oscillation::~Oscillation()
{
    dangle.clear();
    ddangle.clear();
}

Oscillation::Oscillation(const Oscillation &d) : dangle(d.dangle),
                                    ddangle(d.ddangle)
{
    m_domain = d.m_domain;
    m_codomain = d.m_codomain;
}

Oscillation& Oscillation::Oscillation::operator=(const Oscillation &d)
{
    m_domain = d.m_domain;
    m_codomain = d.m_codomain;
    dangle = d.dangle;
    ddangle = d.ddangle;

    return *this;
}

// todo refactor
void Oscillation::insertAllFieldsFromTo(const Oscillation A, size_t from, size_t to)
{
    m_domain.insert(timeBegin(), A.timeBegin() + from, A.timeBegin() + to);
    m_codomain.insert(angleBegin(), A.angleBegin() + from, A.angleBegin() + to);
    dangle.insert(dangleBegin(), A.dangleBegin() + from, A.dangleBegin() + to);
    ddangle.insert(ddangleBegin(), A.ddangleBegin() + from, A.ddangleBegin() + to);
}

// BASIC STL like
void Oscillation::reserve(const size_t s)
{
    AngleHistory::reserve(s);
    dangle.reserve(s);
    ddangle.reserve(s);
}

void Oscillation::push_back(const double &t, const double &a, const double &da, const double &dda)
{
    AngleHistory::push_back(t, a);
    dangle.push_back(da);
    ddangle.push_back(dda);
}

void Oscillation::clear()
{
    AngleHistory::clear();
    dangle.clear();
    ddangle.clear();
}

void Oscillation::print() const // todo override
{
    for (size_t i = 0; i < size(); i++)
    {
        std::cerr << *this;
    }
}

void Oscillation::write(const std::string &file_name) const
{
    std::cout << "Oscillation write to file: " << file_name << "\n";

    this->info();

    std::ofstream fout(file_name);

    fout << *this;
    /*for(size_t i = 0; i < this->size(); i++){
        fout
            << m_domain[i] << "\t"
            << m_codomain[i] << "\t"
            << dangle[i] << "\t"
            << ddangle[i] << "\n";
        //std::cout << time[i] << std::endl;
    }*/
    fout.close();
}

/*
 *   Load Oscillation file saved in 'write"-form
 *   TODO refactor
 */
bool Oscillation::loadFile(const std::string &fileName,
                      oscillation_helpers::FileType mode)
{
    this->clear();

    switch (mode)
    {
        case oscillation_helpers::DEFAULT:
        {
            // try to analyze file
            if (oscillation_helpers::isOscillationFile(fileName)) // todo check for words in string
                mode = oscillation_helpers::TIME_ANGLE_DANGLE_DDANGLE;
            else
                mode = oscillation_helpers::TIME_ANGLE;
        }

        case oscillation_helpers::TIME_ANGLE:
        {
            std::cout << "LOADING "
                    << "TIME_ANGLE\n";
            std::cout << "loading raw oscillation file \"" << fileName << "\"\n";

            Oscillation result(fileName);

            // todo rewrite
            AngleHistory::setTime(result.m_domain);
            AngleHistory::setAngle(result.m_codomain);

            dangle = result.dangle;
            ddangle = result.ddangle;

            break;
        }
        case oscillation_helpers::TIME_ANGLE_DANGLE:
        {
            std::cout << "LOADING "
                    << "TIME_ANGLE_DANGLE\n";
            std::ifstream fin(fileName);

            if (!fin.is_open())
            {
                std::cerr << "failed to open file " << fileName << "\n";
                return false;
            }

            double tBuff, aBuff, daBuff;
            while (!fin.eof())
            {
                fin >> tBuff >> aBuff >> daBuff;

                this->push_back(tBuff, aBuff, daBuff, 0);
            }

            AngleHistory::calculateTimeStep();
            double h = AngleHistory::getTimeStep();

            /// ddangle calc
            // for(size_t i = 0; i < size(); ++i)
            //     ddangle.emplace_back(derevativeOrd2N2(angle, h, i));

            /// dangle calc
            double d = 0.0;
            std::cout << "SIZE = " << size() << "\n";
            ddangle.clear();
            ddangle.reserve(size());
            for (size_t i = 0; i < size(); ++i)
            {
                d = derevativeOrd1N2(dangle, h, i);
                // std::cout << "result = " << d << "\n";
                ddangle.emplace_back(d);
            }

            for (size_t i = 0; i < size(); ++i)
            {

                std::cout << "ddangle = " << ddangle.at(i) << "\n";
            }
            fin.close();
            break;
        }
        case oscillation_helpers::TIME_ANGLE_DANGLE_DDANGLE:
        {
            std::cout << "LOADING "
                    << "TIME_ANGLE_DANGLE_DDANGLE\n";
            std::cout << "loading prepared oscillation file \"" << fileName << "\"\n";

            std::ifstream fin(fileName);

            if (!fin.is_open())
            {
                std::cerr << "failed to open file " << fileName << "\n";
                return false;
            }

            double tBuff, aBuff, daBuff, ddaBuff;
            while (!fin.eof())
            {
                fin >> tBuff >> aBuff >> daBuff >> ddaBuff;

                // std::cout << tBuff << "\t" << aBuff << "\t" << daBuff << "\n" << ddaBuff << "\n";

                this->push_back(tBuff, aBuff, daBuff, ddaBuff);
            }

            fin.close();
            break;
        }
    }

    this->info();

    return true;
}

/// SPECIAL
// use vector helpers
void Oscillation::scaleDAngle(const double &factor)
{
    for (size_t i = 0; i < dangle.size(); i++)
        dangle[i] *= factor;
}

void Oscillation::scaleDDAngle(const double &factor)
{
    for (size_t i = 0; i < ddangle.size(); i++)
        ddangle[i] *= factor;
}

/// OTHER
void  Oscillation::info() const
{
    std::cout << "Oscillation object \n"
              << "\tdangle size: " << dangle.size() << "\n"
              << "\tddangle size: " << ddangle.size() << "\n";
    AngleHistory::info();
}

