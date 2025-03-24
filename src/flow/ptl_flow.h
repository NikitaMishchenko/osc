#pragma once

#include <vector>

#include "flow/wt_flow.h"

class FirstTable
{
public:
    std::vector<int> N;
    std::vector<int> isTestStarted;
    std::vector<double> TX;
    std::vector<double> P0b;
    std::vector<double> PC_M;
    std::vector<double> AP;
    std::vector<double> AI;
    std::vector<double> P0_M;
    std::vector<double> pkt_ls;
    std::vector<double> t1;
    std::vector<double> t2;

    void push_back(int _N,
                   int _isTestStarted,
                   double _TX,
                   double _P0b,
                   double _PC_M,
                   double _AP,
                   double _AI,
                   double _P0_M,
                   double _pkt_ls,
                   double _t1,
                   double _t2)
    {
        N.push_back(_N);
        isTestStarted.push_back(_isTestStarted);
        TX.push_back(_TX);
        P0b.push_back(_P0b);
        PC_M.push_back(_PC_M);
        AP.push_back(_AP);
        AI.push_back(_AI);
        P0_M.push_back(_P0_M);
        pkt_ls.push_back(_pkt_ls);
        t1.push_back(_t1);
        t2.push_back(_t2);
    }

    static FirstTable parse(std::ifstream &fin)
    {
        findFirstTable(fin);

        std::string buffString;

        // read first table
        FirstTable firstTable;

        while (1)
        {
            fin >> buffString;

            if ("______________________________________________________________________________________" == buffString)
            {
                break;
            }

            int N = std::stoi(buffString);

            fin >> buffString;
            if (!(buffString == "1" || buffString == "2" || buffString == "3"))
                throw "bad format of ptl data in row \"isTestStarted\"";

            int isTestStarted = std::stoi(buffString);

            double TX;
            fin >> TX;

            double P0b;
            fin >> P0b;

            double PC_M;
            fin >> PC_M;

            double AP;
            fin >> AP;

            double AI;
            fin >> AI;

            double P0_M;
            fin >> P0_M;

            double pkt_ls;
            fin >> pkt_ls;

            double t1;
            fin >> t1;

            double t2;
            fin >> t2;

            firstTable.push_back(N,
                                 isTestStarted,
                                 TX,
                                 P0b,
                                 PC_M,
                                 AP,
                                 AI,
                                 P0_M,
                                 pkt_ls,
                                 t1,
                                 t2);
        }

        return firstTable;
    }

private:
    static void findFirstTable(std::ifstream &fin)
    {
        std::string buffString;

        // find first table
        // N          ñèíõð           TX          P0b         PC_M           AP           AI         P0_M       pkt_ls     t1 (ñåê)     t2 (ñåê)
        {
            const std::string stringMarker = "----------------------------------------------";

            int i = 0;
            while (i < 3)
            {
                if (stringMarker == buffString)
                {
                    i++;
                }

                fin >> buffString;
            }

            std::getline(fin, buffString);
        }
    }
};

struct SecondTable
{
public:

    void push_back(double _pkt_r,
                   double _pKD,
                   double _UP,
                   double _AL,
                   double _ALF,
                   double _M,
                   double _M_r,
                   double _M_ls)
    {
        pkt_r.push_back(_pkt_r);
        pKD.push_back(_pKD);
        UP.push_back(_UP);
        AL.push_back(_AL);
        ALF.push_back(_ALF);
        M.push_back(_M);
        M_r.push_back(_M_r);
        M_ls.push_back(_M_ls);
    }

    static SecondTable parse(std::ifstream &fin, const std::vector<int> N)
    {       
        std::string buffString;

        // find second table
        // N          pkt_r          pKD           UP           AL          ALF            M          M_r         M_ls     t1 (ñåê)     t2 (ñåê)
        {
            std::getline(fin, buffString);
            std::getline(fin, buffString);
        }
        
        SecondTable secondTable;

        int buffInt;
        double buffDouble;

        for (int i = 0; i < N.size(); i++)
        {
            // N
            fin >> buffInt;

            double pkt_r;
            fin >> pkt_r;

            double pKD;
            fin >> pKD;

            double UP;
            fin >> UP;

            double AL;
            fin >> AL;

            double ALF;
            fin >> ALF;

            double M;
            fin >> M;

            double M_r;
            fin >> M_r;

            double M_ls;
            fin >> M_ls;


            // f1
            fin >> buffDouble;
            // f2
            fin >> buffDouble;
            
            secondTable.push_back(pkt_r,
                                  pKD,
                                  UP,
                                  AL,
                                  ALF,
                                  M,
                                  M_r,
                                  M_ls);
        }

        return secondTable;
    }

    std::vector<double> pkt_r;
    std::vector<double> pKD;
    std::vector<double> UP;
    std::vector<double> AL;
    std::vector<double> ALF;
    std::vector<double> M;
    std::vector<double> M_r;
    std::vector<double> M_ls;


    private:
};


inline std::vector<wt_flow::Flow>
parsePtlFile(const std::string &fileName,
             const int maxTestCountsScinceAnglePolling = 999,
             const int skipCounts = 0)
{
    std::ifstream fin(fileName);

    std::string buffString;
    double buffDouble;
    int buffInt;

    FirstTable firstTable = FirstTable::parse(fin);

    SecondTable secondTable = SecondTable::parse(fin, firstTable.N);

    // find third table
    // N             Re            Q     t1 (ñåê)     t2 (ñåê)
    {
        std::getline(fin, buffString); // ___...
        std::getline(fin, buffString); // \r
        std::getline(fin, buffString); // header
    }

    // N
    std::vector<double> Re;
    std::vector<double> Q;
    // t1
    // t2

    {
        for (int i = 0; i < firstTable.N.size(); i++)
        {
            // N
            fin >> buffInt;

            fin >> buffDouble;
            Re.push_back(buffDouble);

            fin >> buffDouble;
            Q.push_back(buffDouble);

            // f1
            fin >> buffDouble;
            // f2
            fin >> buffDouble;
        }
    }

    // find forth table
    // N          F(TX)       F(P0b)      F(PC_M)        F(AP)        F(AI)      F(P0_M)    F(pkt_ls)     F(pkt_r)     t1 (ñåê)     t2 (ñåê)
    {
        std::getline(fin, buffString); // __...
        std::getline(fin, buffString); // \r
        std::getline(fin, buffString); // header
    }

    // N
    std::vector<double> F_TX;
    std::vector<double> F_P0b;
    std::vector<double> F_PC_M;
    std::vector<double> F_AP;
    std::vector<double> F_AI;
    std::vector<double> F_p0_V;
    std::vector<double> F_pkt_ls;
    std::vector<double> F_pkt_r;
    // t1
    // t2

    {
        for (int i = 0; i < firstTable.N.size(); i++)
        {
            // N
            fin >> buffInt;

            fin >> buffDouble;
            F_TX.push_back(buffDouble);

            fin >> buffDouble;
            F_P0b.push_back(buffDouble);

            fin >> buffDouble;
            F_PC_M.push_back(buffDouble);

            fin >> buffDouble;
            F_AP.push_back(buffDouble);

            fin >> buffDouble;
            F_AI.push_back(buffDouble);

            fin >> buffDouble;
            F_p0_V.push_back(buffDouble);

            fin >> buffDouble;
            F_pkt_ls.push_back(buffDouble);

            fin >> buffDouble;
            F_pkt_r.push_back(buffDouble);

            // f1
            fin >> buffDouble;
            // f2
            fin >> buffDouble;
        }
    }

    // find fifth table
    // N         F(pKD)        F(UP)       F(ALF)     t1 (ñåê)     t1 (ñåê)     t2 (ñåê)
    {
        std::getline(fin, buffString); // \r
        std::getline(fin, buffString); // ___...
        std::getline(fin, buffString); // header
    }

    // N
    std::vector<double> F_pkD;
    std::vector<double> F_UP;
    std::vector<double> F_ALF;
    // t1
    // t1
    // t2

    {
        for (int i = 0; i < firstTable.N.size(); i++)
        {
            // N
            fin >> buffInt;

            fin >> buffDouble;
            F_pkD.push_back(buffDouble);

            fin >> buffDouble;
            F_UP.push_back(buffDouble);

            fin >> buffDouble;
            F_ALF.push_back(buffDouble);

            // t1
            fin >> buffDouble;
            // t1
            fin >> buffDouble;
            // t2
            fin >> buffDouble;
        }
    }

    std::vector<wt_flow::Flow> paresedFlow;

    for (int i = 0; i < firstTable.N.size(); i++)
    {
        if (3 == firstTable.isTestStarted.at(i))
            paresedFlow.push_back(wt_flow::Flow(Q.at(i), secondTable.M.at(i), F_TX.at(i), Re.at(i))); // dynamic pressure calculated here
    }

    return paresedFlow;
}

inline wt_flow::Flow
averageFLowData(const std::vector<wt_flow::Flow> flowVector)
{
    if (flowVector.empty())
        throw "no data in flowVector!";

    wt_flow::Flow resultFlow;

    std::vector<double> density;
    std::vector<double> dynamicPressure;
    std::vector<double> velocity;
    std::vector<double> temperature;
    std::vector<double> mach;
    std::vector<double> reynolds;

    for (const auto &flow : flowVector)
    {
        density.push_back(flow.getDensity());
        dynamicPressure.push_back(flow.getDynamicPressure());
        velocity.push_back(flow.getVelocity());
        temperature.push_back(flow.getTemperature());
        mach.push_back(flow.getMach());
        reynolds.push_back(flow.getReynolds());
    }

    // border is Mach Number 1%
    int sizeToAvg = flowVector.size();
    {
        double machInitial = mach.front();
        double machCum;
        // find size to calculate avg parameters
        for (int i = 1; i < flowVector.size(); i++)
        {
            machCum = std::accumulate(mach.begin(), mach.begin() + i, 0.0) / i;

            const double ratio = std::abs(1.0 - machCum / mach.at(i));

            if (ratio > 0.01)
            {
                sizeToAvg = i;
                // std::cout << "propper params to calc flow after start of test is " << sizeToAvg << std::endl;
                break;
            }

            machInitial = machCum;
        }
    }

    const double densityFinal = std::accumulate(density.begin(), density.begin() + sizeToAvg, 0.0) / sizeToAvg;
    const double dynamicPressureFinal = std::accumulate(dynamicPressure.begin(), dynamicPressure.begin() + sizeToAvg, 0.0) / sizeToAvg;
    const double velocityFinal = std::accumulate(velocity.begin(), velocity.begin() + sizeToAvg, 0.0) / sizeToAvg;
    const double temperatureFinal = std::accumulate(temperature.begin(), temperature.begin() + sizeToAvg, 0.0) / sizeToAvg;
    const double machFinal = std::accumulate(mach.begin(), mach.begin() + sizeToAvg, 0.0) / sizeToAvg;
    const double reynoldsFinal = std::accumulate(reynolds.begin(), reynolds.begin() + sizeToAvg, 0.0) / sizeToAvg;

    return wt_flow::Flow(densityFinal,
                         velocityFinal,
                         temperatureFinal,
                         dynamicPressureFinal,
                         machFinal,
                         reynoldsFinal);
}
