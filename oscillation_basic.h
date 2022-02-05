#pragma once

#include <iostream>

#include <fstream>

#include <vector>
//#include <deque>
#include <string>

#include <iterator>
#include <algorithm>


#include "AngleHistory.h"

enum LOG_MODE{
    LOG_ON,
    LOG_OFF
};



class oscillation : public AngleHistory
{
public:
    std::vector<double> dangle;
    std::vector<double> ddangle;

    oscillation(): AngleHistory(), dangle(), ddangle()
    {
        std::cerr << "default oscillation()\n";
    }

    oscillation(const std::string file_name): AngleHistory(file_name)//, dangle()
    {
        std::cout << "oscillation( " << file_name << ") constructor\n";
        std::cout << "angle size = " << angle.size() << "\n";
        if(0 != angle.size())
        {
                //std::cout << angle.size() << std::endl;
                //std::cout << dangle.size() << std::endl;
            dangle.reserve(this->size());

            double h = time[1] - time[0];

            ///dangle calc
                dangle.push_back(0);
                for(size_t i = 1; i < size()-1; ++i)
                {
                    dangle.push_back(angle[i-1] - angle[i+1]);
                    dangle[i] /= 2.0*h;
                }
                dangle.push_back(0);

            ///ddangle calc
                ddangle.push_back(0);
                for(size_t i = 1; i < size()-1; ++i)
                {
                    ddangle.push_back(dangle[i-1] - dangle[i+1]);
                    ddangle[i] /= 2.0*h;
                }
                ddangle.push_back(0);
        }
        else
        {
            std::cout << "empty AngleHistory -> empty oscillation \n";
            dangle.reserve(0);
            ddangle.reserve(0);
        }

    }

    ~oscillation(){
        dangle.clear();
        ddangle.clear();
    }

    oscillation(const oscillation& d) :
                                        dangle(d.dangle),
                                        ddangle(d.ddangle)
    {
        time = d.time;
        angle = d.angle;
        std::cout << "oscillation::copy constructor\n";

    }

    oscillation& operator= (const oscillation& d)
    {
        std::cout << "oscillation::operator=\n";

        //time = d.time;
        //angle = d.angle;
        dangle = d.dangle;
        ddangle = d.ddangle;

        return *this;
    }


    //BASIC
    void push_back(const double& t, const double& a, const double& da, const double& dda)
    {
        time.push_back(t);
        angle.push_back(a);
        dangle.push_back(da);
        ddangle.push_back(dda);
    }

    void clear()
    {
        time.clear();
        angle.clear();
        dangle.clear();
        ddangle.clear();
    }

    const std::vector<double> get_angle() const
    {
        return angle;
    }


    const double get_angle(int i) const
    {
        return angle[i];
    }

    const double get_dangle(int i) const
    {
        return dangle[i];
    }

    double get_ddangle(int i) const
    {
        return dangle[i];
    }


    //IO
    void print() const
    {
        for(size_t i = 0; i < size(); i++)
        {
            std::cerr
                << time[i] << "\t"
                << angle[i] << "\t"
                << dangle[i] << "\t"
                << ddangle[i] << "\n";
        }

    }

    void write(std::string file_name)
    {
        std::cout << "oscillation write(file_name)\n";
        this->info();
        //angle_history.print();
        std::ofstream fout(file_name);
        for(size_t i = 0; i < this->size(); i++){
            fout
                << time[i] << "\t"
                << angle[i] << "\t"
                << dangle[i] << "\t"
                << ddangle[i] << "\n";
            //std::cout << time[i] << std::endl;
        }
        fout.close();
    }

    /*
    *   Load oscillation file saved in 'write"-form
    */
    bool loadFile(std::string file_name)
    {
        this->clear();

        std::ifstream fin( file_name);

        if( !fin.is_open())
        {
            return false;
        }

        double tBuff, aBuff, daBuff, ddaBuff;
        while(!fin.eof())
        {
            fin >> tBuff >> aBuff >> daBuff >> ddaBuff;

            this->push_back(tBuff, aBuff, daBuff, ddaBuff);
        }

        return true;
    }

    friend std::ostream& operator<< (std::ostream& out, const oscillation& D)
    {
        for(size_t i = 0; i < D.size(); i++)
            out << D.time[i] << "\t"
                << D.angle[i] << "\t"
                << D.dangle[i] << "\t"
                << D.ddangle[i] << "\n";
        return out;
    }

    const size_t size() const override {return angle.size();}

    void info() override {
        std::cout << "oscillation object \nsize = ";
        std::cout << this->size() << std::endl;
    }
};
