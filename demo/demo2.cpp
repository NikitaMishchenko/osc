#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <string>

#include <boost/filesystem.hpp>

#include "flow/wt_flow.h"
// #include "flow/parse_ptl.h"

std::vector<wt_flow::Flow> parsePtlFile(const std::string &fileName, const int maxTestCountsScinceAnglePolling = 999, const int skipCounts = 0)
{
    std::ifstream fin(fileName);

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

    // read first table
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

    double buffDouble;
    int buffInt;

    while (1)
    {
        fin >> buffString;

        if ("______________________________________________________________________________________" == buffString)
        {
            break;
        }

        N.push_back(std::stoi(buffString));
        fin >> buffInt;

        isTestStarted.push_back(buffInt);

        fin >> buffDouble;
        TX.push_back(buffDouble);

        fin >> buffDouble;
        P0b.push_back(buffDouble);

        fin >> buffDouble;
        PC_M.push_back(buffDouble);

        fin >> buffDouble;
        AP.push_back(buffDouble);

        fin >> buffDouble;
        AI.push_back(buffDouble);

        fin >> buffDouble;
        P0_M.push_back(buffDouble);

        fin >> buffDouble;
        pkt_ls.push_back(buffDouble);

        fin >> buffDouble;
        t1.push_back(buffDouble);

        fin >> buffDouble;
        t2.push_back(buffDouble);
    }

    // find second table
    // N          pkt_r          pKD           UP           AL          ALF            M          M_r         M_ls     t1 (ñåê)     t2 (ñåê)
    {
        std::getline(fin, buffString);
        std::getline(fin, buffString);
    }

    // N
    std::vector<double> pkt_r;
    std::vector<double> pKD;
    std::vector<double> UP;
    std::vector<double> AL;
    std::vector<double> ALF;
    std::vector<double> M;
    std::vector<double> M_r;
    std::vector<double> M_ls;
    // t1
    // t2

    {
        for (int i = 0; i < N.size(); i++)
        {
            // N
            fin >> buffInt;

            fin >> buffDouble;
            pkt_r.push_back(buffDouble);

            fin >> buffDouble;
            pKD.push_back(buffDouble);

            fin >> buffDouble;
            UP.push_back(buffDouble);

            fin >> buffDouble;
            AL.push_back(buffDouble);

            fin >> buffDouble;
            ALF.push_back(buffDouble);

            fin >> buffDouble;
            M.push_back(buffDouble);

            fin >> buffDouble;
            M_r.push_back(buffDouble);

            fin >> buffDouble;
            M_ls.push_back(buffDouble);

            // f1
            fin >> buffDouble;
            // f2
            fin >> buffDouble;
        }
    }

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
        for (int i = 0; i < N.size(); i++)
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
        for (int i = 0; i < N.size(); i++)
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
        for (int i = 0; i < N.size(); i++)
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

    for (int i = 0; i < N.size(); i++)
    {
        if (3 == isTestStarted.at(i))
            paresedFlow.push_back(wt_flow::Flow(Q.at(i), M.at(i), F_TX.at(i), Re.at(i)));
    }

    return paresedFlow;
}

void parseAndSaveFlow(const boost::filesystem::path& ptlFlowFile)
{
    std::vector<wt_flow::Flow> flowVector = parsePtlFile(ptlFlowFile.string() + ".ptl");

    std::vector<double> density;
    std::vector<double> dynamicPressure;
    std::vector<double> velocity;
    std::vector<double> temperature;
    std::vector<double> mach;
    std::vector<double> reynolds;

    {
        std::ofstream fout(ptlFlowFile.string() + "_parsed_flow");

        for (const auto &flow : flowVector)
        {
            fout << flow.getDensity() << " "
                 << flow.getDynamicPressure() << " "
                 << flow.getVelocity() << " "
                 << flow.getTemperature() << " "
                 << flow.getMach() << " "
                 << flow.getReynolds() << "\n";

            density.push_back(flow.getDensity());
            dynamicPressure.push_back(flow.getDynamicPressure());
            velocity.push_back(flow.getVelocity());
            temperature.push_back(flow.getTemperature());
            mach.push_back(flow.getMach());
            reynolds.push_back(flow.getReynolds());
        }
    }

    // border is Mach Number 1%

    if (flowVector.empty())
        throw "no data after parsing!";

    int sizeToAvg = flowVector.size();
    {
        double machInitial = mach.front();
        double machCum;
        // find size to calculate avg parameters
        for (int i = 1; i < flowVector.size(); i++)
        {
            machCum = std::accumulate(mach.begin(), mach.begin() + i, 0.0)/i;

            const double ratio = std::abs(1.0 - machCum/mach.at(i));

            if (ratio > 0.01)
            {
                sizeToAvg = i;
                std::cout << "propper params to calc flow after start of test is " << sizeToAvg << std::endl;
                break;
            }

            machInitial = machCum; 
        }
    }

    const double densityFinal = std::accumulate(density.begin(), density.begin() + sizeToAvg, 0.0)/sizeToAvg;
    const double dynamicPressureFinal = std::accumulate(dynamicPressure.begin(), dynamicPressure.begin() + sizeToAvg, 0.0)/sizeToAvg;
    const double velocityFinal = std::accumulate(velocity.begin(), velocity.begin() + sizeToAvg, 0.0)/sizeToAvg;
    const double temperatureFinal = std::accumulate(temperature.begin(), temperature.begin() + sizeToAvg, 0.0)/sizeToAvg;
    const double machFinal = std::accumulate(mach.begin(), mach.begin() + sizeToAvg, 0.0)/sizeToAvg;
    const double reynoldsFinal = std::accumulate(reynolds.begin(), reynolds.begin() + sizeToAvg, 0.0)/sizeToAvg;


    wt_flow::Flow flowCalculated(wt_flow::Flow(densityFinal,
                                               velocityFinal,
                                               temperatureFinal,
                                               dynamicPressureFinal,
                                               machFinal,
                                               reynoldsFinal));

    {
        flowCalculated.saveFile(ptlFlowFile.string() + ".flow");
    }
}

int main(int argc, char **argv)
{
    boost::filesystem::path basePath = "/home/mishnic/data/data_proc/sphere_cone_M1.75/flow_ptl/";
    
    for (int i = 4461; i < 4475; i++)
    {
        std::string coreName = std::to_string(i); //"4474";

        boost::filesystem::path ptlFlowFile = basePath / coreName;

        parseAndSaveFlow(ptlFlowFile);
    }

}
