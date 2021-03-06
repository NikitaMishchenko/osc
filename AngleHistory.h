#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class AngleHistory
{
public:
    std::vector<double> angle;
    std::vector<double> time;

    AngleHistory(){
        std::cerr << "default AngleHistory()\n";
    }

    virtual void load_row(std::string file_name)
    {
        std::ifstream fin(file_name);
        while(!fin.eof())
        {
            double b_angle, b_time;
                fin >> b_time >> b_angle;
                    angle.push_back(b_angle);
                    time.push_back(b_time);
//                std::cerr
//                        << "angle = " << angle.back()
//                        << "angle = " << angle.back()

        }
        fin.close();
    }

    AngleHistory(std::string file_name){
        std::cerr << "AngleHistory(file_name) constructor\n";
        this->load_row(file_name);
            this->info();
    }

    ~AngleHistory(){
        angle.clear();
        time.clear();
    }

    virtual void print(){
        for(size_t i = 0; i < angle.size(); i++)
        {
            std::cout << time[i] << "\t" << angle[i] << std::endl;
        }
    }

    virtual size_t size(){ return angle.size();}

    virtual void info()
    {
        std::cout << "AngleHistory object \nsize = ";
        std::cout << angle.size() << std::endl;
    }
};

