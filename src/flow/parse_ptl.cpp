#include <iostream>
#include <fstream>

#include <string>
#include <vector>

#include <numeric>

#include "wt_flow.h"

namespace wt_flow
{
    /// todo вынести в сторонюю библиотеку парс трубы Т-313
    namespace
    {
        /**
        *     todo  разбивать строку из данных элементов
        */
        enum ptlTable
        {
                N_smth_Tx_POb_PC_M_AP_AI_P0_M_pkt_ls_t1_t2 = 1,
                N_pkt_r_pKD_UP_AL_ALF_M_M_r_M_ls_t1_t2,
                N_Re_Q_t1_t2
        };


        /**
        *   Содержит строку с описанием таблицы (по ней определяется начало данных таблицы)
        */
        bool parsedContains(const std::string& str, const int table)
        {
            /**
            *   make special object of each table to perform better parsing
            *   ex: table discription, tavble, ended, tableDtat(data1, data2, etc...)
            */
            if (N_smth_Tx_POb_PC_M_AP_AI_P0_M_pkt_ls_t1_t2 == table)
            return (
                    (str.find("N")  != std::string::npos) &&
                    (str.find("TX")  != std::string::npos) &&
                    (str.find("P0b")  != std::string::npos) &&
                    (str.find("PC_M") != std::string::npos) &&
                    (str.find("AP") != std::string::npos) &&
                    (str.find("AI") != std::string::npos) &&
                    (str.find("P0_M") != std::string::npos) &&
                    (str.find("pkt_ls") != std::string::npos) &&
                    (str.find("t1") != std::string::npos) &&
                    (str.find("t2") != std::string::npos)
                    );

            if (N_pkt_r_pKD_UP_AL_ALF_M_M_r_M_ls_t1_t2 == table)
            return (
                    (str.find("N")  != std::string::npos) &&
                    (str.find("pkt_r")  != std::string::npos) &&
                    (str.find("pKD")  != std::string::npos) &&
                    (str.find("UP") != std::string::npos) &&
                    (str.find("AL") != std::string::npos) &&
                    (str.find("ALF") != std::string::npos) &&
                    (str.find("M") != std::string::npos) &&
                    (str.find("M_r") != std::string::npos) &&
                    (str.find("M_ls") != std::string::npos) &&
                    (str.find("t1") != std::string::npos) &&
                    (str.find("t2") != std::string::npos)
                    );

            if (N_Re_Q_t1_t2 == table)
            return (
                    (str.find("N")  != std::string::npos) &&
                    (str.find("Re") != std::string::npos) &&
                    (str.find("Q")  != std::string::npos) &&
                    (str.find("N")  != std::string::npos) &&
                    (str.find("t1") != std::string::npos) &&
                    (str.find("t1") != std::string::npos)
                    );

            return true; /// todo return errcode
        }
    } // namespace


    void parseToTableBegining(std::ifstream& fin, const int ptlTableDiscription, int& counter)
    {
        std::string buff_s;
        while (!parsedContains(buff_s, ptlTableDiscription))
        {
            std::getline(fin, buff_s);
            counter++ ;
        }
    }


    void skipStrings(std::ifstream& fin, const int skipCounter)
    {
        std::string buff_s;
        for (int i = 0; i < skipCounter; i++)
            std::getline(fin, buff_s);
    }


    /**
    *   Parse .ptl file (T-313 wt protocol)
    */
    bool parsePTLfile(const std::string& fileName, Flow& flowData, const int maxTestCountsScinceAnglePolling)
    {
        std::cout << "parsePTLfile:: started\n";
        std::ifstream fin(fileName);

        if (!fin.is_open())
        {
            std::cerr << "Can't open file! aborting\n";
            return false;
        }
        std::cout << "parsePTLfile:: file " << fileName << " is opened\n";

        ////////////////////////////////////////////////////////////////////////////////////////
        /// time stamp of Angle Polling
        ////////////////////////////////////////////////////////////////////////////////////////
        std::string buff_s;
        int counter = 1;
        parseToTableBegining(fin, N_smth_Tx_POb_PC_M_AP_AI_P0_M_pkt_ls_t1_t2, counter);
        /// нашли данные таблицы с отметками времени сбора данных колебаний модели
        std::cout << "FOUND TABLE of isAnglePolling\n";

        int pollingAngleFirstCount = 0;
        int polingAngleTotalCounts = 0;

        int isAnglePolling = 2; //2 no, 3 yes
        int buff_i;
        fin >> buff_i; // N

        double buff_Tx;
        double buff_t2;
        while ( isAnglePolling != 3 && buff_s != "______________________________________________________________________________________")
        {
            fin >> isAnglePolling; /// 3 Angle is polling

            fin >> buff_Tx;

            for(int i = 0; i < 7; i++)
                fin >> buff_s;

            fin >> buff_t2; // t2
            fin >> buff_s; // N of the next string

            counter++ ;
            pollingAngleFirstCount++;
        }

        std::cout << "FOUND DATA of isAnglePolling\n";

        std::vector<double> timeAnglePolling;
        std::vector<double> Tx;

        timeAnglePolling.push_back(buff_t2);
        Tx.push_back(buff_Tx);

        while ( isAnglePolling == 3 && buff_s != "______________________________________________________________________________________")
        {
            fin >> isAnglePolling;

            fin >> buff_Tx;
                Tx.push_back(buff_Tx);

            for(int i = 0; i < 7; i++)
                fin >> buff_s;


            fin >> buff_t2; // t2
            timeAnglePolling.push_back(buff_t2);

            fin >> buff_s; // N of the next string

            counter++ ;
            polingAngleTotalCounts++;
        }
        buff_s.clear();


        ////////////////////////////////////////////////////////////////////////////////////////
        /// MACH
        ////////////////////////////////////////////////////////////////////////////////////////
        parseToTableBegining(fin, N_pkt_r_pKD_UP_AL_ALF_M_M_r_M_ls_t1_t2, counter);
        skipStrings(fin, pollingAngleFirstCount);

        double buff_M_r;
        std::vector<double> M_r;
        //while ( buff_s != "______________________________________________________________________________________")
        for (int i = 0; i < polingAngleTotalCounts; i++)
        {
            for(int i = 0; i < 7; i++)
                fin >> buff_s;

            fin >> buff_M_r; // M_r
                M_r.push_back(buff_M_r);

            for(int i = 0; i < 3; i++)
                fin >> buff_s; // N of the next string

            counter++ ;
        }


        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Re, Q, t
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        parseToTableBegining(fin, N_Re_Q_t1_t2, counter);

        std::vector<double> Re;
        std::vector<double> q;
        std::vector<double> t;


        std::ofstream fout(fileName + "_parsed");
        int testCounts = 0;
        double buff_Re, buff_q, buff_t1;
        do
        {
            fin >> buff_s  // N
                >> buff_Re; // Re
            fin >> buff_q; // q
            fin >> buff_t1; // t1
            fin >> buff_s; // t2


            if (buff_t1 > timeAnglePolling.front() &&
                buff_t1 < timeAnglePolling.back())
            {
                /// CAUTION!!!
                q.push_back(buff_q*GRAVITATIONAL_ACCELERATION);
                Re.push_back(buff_Re);
                t.push_back(buff_t1);

                fout << t.back() << "\t"
                    << q.back() << "\t"
                    << Re.back() << "\t"
                    << Tx.at(testCounts) << "\n";

                testCounts++;
                if (testCounts > maxTestCountsScinceAnglePolling)
                {
                    break;
                }
            }

        } while( buff_s != "______________________________________________________________________________________");

        fin.close();
        fout.close();


        /// fit result flow
        flowData.setDynamicPressure(std::reduce(q.begin(), q.begin()+testCounts) / testCounts);
        flowData.setReynolds(std::reduce(Re.begin(), Re.begin()+testCounts) / testCounts);

        flowData.setT0(std::reduce(Tx.begin(), Tx.begin()+testCounts) / testCounts);
        flowData.setMach(std::reduce(M_r.begin(), M_r.begin()+testCounts) / testCounts);

        flowData.calculateVelocity();
        flowData.calculateDensity();

        return true;
    }

} // namespace wt_flow
