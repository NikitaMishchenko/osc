#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class AngleHistory
{
public:

    AngleHistory()
    {}

    AngleHistory(const std::vector<double>& timeIn, const std::vector<double>& angleIn) : m_time(timeIn), m_angle(angleIn)
    {}

    AngleHistory(const std::string& file_name)
    {
        std::cerr << "AngleHistory( " << file_name << ") constructor\n";

        this->loadRaw(file_name);
            this->info();
    }

    virtual ~AngleHistory()
    {
        m_angle.clear();
        m_time.clear();
    }

    //copy
    AngleHistory(const AngleHistory& d) : m_time(d.m_time), m_angle(d.m_angle)
    {
        std::cout << "AngleHistory copy constructor\n";
    }

    AngleHistory& operator= (const AngleHistory& d)
    {
        m_time = d.m_time;
        m_angle = d.m_angle;

        return *this;
    }


    std::vector<double> getAngle(){ return m_angle;}

    std::vector<double> getTime(){ return m_time;}

    void moveAngle(const double A)
    {
        for(auto& a : m_angle)
            a += A;
    }

    virtual void print()
    {
        std::cout << *this;
    }


    /*
    * TODO make overload for operator>>
    */
    friend std::ostream& operator<< (std::ostream& out, const AngleHistory& D)
    {
        for(size_t i = 0; i < D.size(); i++)
        {
             out << D.m_time[i] << "\t"
                 << D.m_angle[i] << "\t"
                 << "\n";
        }

        return out;
    }

    virtual const size_t size() const { return m_angle.size();}

    virtual void info()
    {
        std::cout << "AngleHistory object \nsize = ";
        std::cout << m_angle.size() << std::endl;
    }

    virtual bool loadRaw(const std::string& file_name)
    {
        std::ifstream fin(file_name);
        std::cout << "trying open file: " << file_name << std::endl;

        if(fin.is_open())
        {
            while(!fin.eof())
            {
                double b_angle, b_time;

                fin >> b_time >> b_angle;

                this->push_back(b_time, b_angle);

            }
            fin.close();
            return true;
        }
        else
        {
            std::cerr << "File did not opened" << std::endl;
        }

        fin.close();
        return true;
    }

protected:

    void push_back(const double time, const double angle)
    {
        m_time.push_back(time);
        m_angle.push_back(angle);
    }

    std::vector<double> m_time;
    std::vector<double> m_angle;
};
