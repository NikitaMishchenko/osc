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

    oscillation(): AngleHistory(), dangle()
    {
        std::cerr << "default oscillation()\n";
    }

    oscillation(const std::string file_name): AngleHistory(file_name)//, dangle()
    {
        std::cout << "oscillation(file_name) constructor\n";
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

    ~oscillation(){
        dangle.clear();
    }

    std::vector<double> get_angle() const{
    return angle;}


    double get_angle(int i){
    return angle[i];}

    double get_dangle(int i){
    return dangle[i];}

    double get_ddangle(int i){
    return dangle[i];}


    void print(){
        for(size_t i = 0; i < size(); i++){
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

    size_t size()override {return angle.size();}

    void info() override {
        std::cout << "oscillation object \nsize = ";
        std::cout << this->size() << std::endl;
    }
};


