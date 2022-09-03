#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class AngleHistory
{
public:
    std::vector<double> time;
    std::vector<double> angle;


    AngleHistory()
    {}

    AngleHistory(const std::vector<double>& timeIn, const std::vector<double>& angleIn) : time(timeIn), angle(angleIn)
    {}

    AngleHistory(const std::string& file_name)
    {
        std::cerr << "AngleHistory( " << file_name << ") constructor\n";

        this->load_row(file_name);
            this->info();
    }

    ~AngleHistory()
    {
        angle.clear();
        time.clear();
    }

    //copy
    AngleHistory(const AngleHistory& d) : time(d.time), angle(d.angle)
    {
        std::cout << "AngleHistory copy constructor\n";
    }

    AngleHistory& operator= (const AngleHistory& d)
    {
        time = d.time;
        angle = d.angle;

        return *this;
    }


    void move_angle(const double A)
    {
        for(auto a : angle)
            a += A;
    }

    virtual void print()
    {
        for(size_t i = 0; i < angle.size(); i++)
        {
            std::cout << time[i] << "\t" << angle[i] << std::endl;
        }
    }


    friend std::ostream& operator<< (std::ostream& out, const AngleHistory& D)
    {
        for(size_t i = 0; i < D.size(); i++)
        {
             out << D.time[i] << "\t"
                 << D.angle[i] << "\t"
                 << "\n";
        }

        return out;
    }

    virtual const size_t size() const { return angle.size();}

    virtual void info()
    {
        std::cout << "AngleHistory object \nsize = ";
        std::cout << angle.size() << std::endl;
    }

private:
    virtual void load_row(const std::string& file_name)
    {
        std::ifstream fin(file_name);
        std::cout << "trying open file: " << file_name << std::endl;

        if(fin.is_open())
        {
            while(!fin.eof())
            {
                double b_angle, b_time;

                fin >> b_time >> b_angle;

                angle.push_back(b_angle);
                time.push_back(b_time);

            }
        }
        else
        {
            std::cerr << "File did not opened" << std::endl;
        }

        fin.close();
    }
};
