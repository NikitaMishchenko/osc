#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <iterator>
#include <algorithm>

#include "AngleHistory.h"
#include "oscillation_helpers.h"

enum LOG_MODE{
    LOG_ON,
    LOG_OFF
};



class Oscillation : public AngleHistory
{
public:
    std::vector<double> dangle;
    std::vector<double> ddangle;

    Oscillation(): AngleHistory(), dangle(), ddangle()
    {
        std::cerr << "default Oscillation()\n";
    }

    Oscillation(const std::string file_name): AngleHistory(file_name)//, dangle()
    {
        std::cout << "Oscillation( " << file_name << ") constructor\n";
        std::cout << "angle size = " << angle.size() << "\n";
        if(0 != angle.size())
        {
            dangle.reserve(this->size());
            ddangle.reserve(this->size());

            const double h = time[1] - time[0];

            ///dangle calc
            for(size_t i = 0; i < size(); ++i)
                dangle.emplace_back(derevativeOrd1N2(angle, h, i));

            ///ddangle calc
            for(size_t i = 0; i < size(); ++i)
                ddangle.emplace_back(derevativeOrd2N2(angle, h, i));

        }
        else
        {
            std::cout << "empty AngleHistory -> empty Oscillation \n";
            dangle.reserve(0);
            ddangle.reserve(0);
        }

    }

    double derevativeOrd1N2(std::vector<double>& func, const double& h, const size_t index)
    {
        //std::cout << index << " derevative \n";
        // left border
        if (0 == index)
            return (-3.0*func.at(0) + 4.0*func.at(1) - func.at(2))/2.0/h;

        // center
        if (index > 0 && index < func.size()-1)
            return (func.at(index+1) - func.at(index-1))/2.0/h; // ~h^2/6(f''')

        //if (index == func.size()-1)
        //    return (func.at())
        // right border
        return 0;
    }

    double derevativeOrd2N2(std::vector<double>& func, const double& h, const size_t index)
    {
        if (index > 0 && index < func.size()-2)
            return (func.at(index+2) - 2.0*func.at(index+1) + func.at(index))/h/h;

        std::cout << "derevativeOrd2N2:: 0 returned\n";
        return 0;
    }

    ~Oscillation(){
        dangle.clear();
        ddangle.clear();
    }

    Oscillation(const Oscillation& d) :
                                        dangle(d.dangle),
                                        ddangle(d.ddangle)
    {
        time = d.time;
        angle = d.angle;
    }

    Oscillation& operator= (const Oscillation& d)
    {
        //time = d.time;
        //angle = d.angle;
        dangle = d.dangle;
        ddangle = d.ddangle;

        return *this;
    }


    //BASIC
    const size_t size() const override {return angle.size();}

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

    const double get_time(int i) const
    {
        return time.at(i);
    }

    const double get_angle(int i) const
    {
        return angle.at(i);
    }

    const double get_dangle(int i) const
    {
        return dangle.at(i);
    }

    double get_ddangle(int i) const
    {
        return dangle.at(i);
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
        std::cout << "Oscillation write(file_name)\n";
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
    *   Load Oscillation file saved in 'write"-form
    */
    bool loadFile(const std::string& fileName)
    {
        this->clear();

        if (oscillation_helpers::isOscillationFile(fileName))
        {
            std::cout << "loading prepared oscillation file \"" << fileName << "\"\n";

            std::ifstream fin( fileName);

            if( !fin.is_open())
            {
                std::cerr << "filed to open file " << fileName << "\n";
                return false;
            }

            double tBuff, aBuff, daBuff, ddaBuff;
            while(!fin.eof())
            {
                fin >> tBuff >> aBuff >> daBuff >> ddaBuff;

                this->push_back(tBuff, aBuff, daBuff, ddaBuff);
            }
        }
        else
        {
            std::cout << "loading raw oscillation file \"" << fileName << "\"\n";

            Oscillation result(fileName);

            // todo rewrite
            time = result.time;
            angle = result.angle;
            dangle = result.dangle;
            ddangle = result.ddangle;
        }

        return true;
    }

    friend std::ostream& operator<< (std::ostream& out, const Oscillation& D)
    {
        for(size_t i = 0; i < D.size(); i++)
            out << D.time[i] << "\t"
                << D.angle[i] << "\t"
                << D.dangle[i] << "\t"
                << D.ddangle[i] << "\n";
        return out;
    }

    /// SPECIAL

    void scaleTime(const double& factor)
    {
        if(time.size() > 0)
            for(size_t i = 0; i < time.size(); i++)
                time[i] *= factor;
    }

    void scaleAngle(const double& factor)
    {
        if(angle.size() > 0)
            for(size_t i = 0; i < angle.size(); i++)
                angle[i] *= factor;
    }

    void scaleDAngle(const double& factor)
    {
        if(dangle.size() > 0)
            for(size_t i = 0; i < dangle.size(); i++)
                dangle[i] *= factor;
    }

    void scaleDDAngle(const double& factor)
    {
        if(ddangle.size() > 0)
            for(size_t i = 0; i < ddangle.size(); i++)
                ddangle[i] *= factor;
    }

    std::vector<double> makeScaledDDAngle(const double& factor)
    {

        std::vector<double> scaledDDAngle;

        if(ddangle.size() > 0)
        {
            scaledDDAngle = ddangle;

            if ( (factor-1.0) < 0.0000000001 &&
                 (1.0-factor) < 0.0000000001)
            {
                return scaledDDAngle;
            }

            for(size_t i = 0; i < scaledDDAngle.size(); i++)
                scaledDDAngle[i] *= factor;
        }

        return scaledDDAngle;
    }

    /// OTHER
    void info() override {
        std::cout << "Oscillation object \nsize = ";
        std::cout << this->size() << std::endl;
    }
};
