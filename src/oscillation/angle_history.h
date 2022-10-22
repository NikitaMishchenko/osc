#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class AngleHistory // todo rename it's better be like TwoVectors // on the higher lvl make it angle and time
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
        //std::cout << "AngleHistory copy constructor\n";
    }

    AngleHistory& operator= (const AngleHistory& d)
    {
        m_time = d.m_time;
        m_angle = d.m_angle;

        return *this;
    }
    
    // std-like
    void clear()
    {
        m_angle.clear();
        m_time.clear();
    }



    std::vector<double> getAngle() const 
    {
        return m_angle;
    }

    double getAngle(int index) const 
    {
        return m_angle.at(index);
    }
    
    void setAngle(size_t index, const double value)
    {
        m_angle.at(index) = value;
    }

    void setAngle(const std::vector<double>& newAngle)
    {
        m_angle = newAngle;
    }

    std::vector<double> getTime() const 
    {
        return m_time;
    }

    double getTime(int index) const 
    {
        return m_time.at(index);
    }
    
    void setTime(size_t index, const double value)
    {
        m_time.at(index) = value;
    }

    void setTime(const std::vector<double>& newTime)
    {
        m_time = newTime;
    }

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
             out << D.m_time.at(i) << "\t"
                 << D.m_angle.at(i) << "\t"
                 << "\n";
        }

        return out;
    }

    virtual const size_t size() const { return m_angle.size();}

    virtual void info() const
    {
        std::cout << "AngleHistory object \n"
                  << "m_time size: " << m_time.size() << "\n"  
                  << "m_angle size: " <<  m_angle.size() << "\n";
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

    virtual void write(const std::string& fileName) const
    {
        std::ofstream fout(fileName);

        fout << *this;

        fout.close();
    }

    void scaleTime(const double& factor)
    {
        if(m_time.size() > 0)
            for(size_t i = 0; i < m_time.size(); i++)
                m_time[i] *= factor;
    }

    void scaleAngle(const double& factor)
    {
        if(m_angle.size() > 0)
            for(size_t i = 0; i < m_angle.size(); i++)
                m_angle[i] *= factor;
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
