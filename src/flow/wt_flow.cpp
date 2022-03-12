#include "wt_flow.h"

namespace wt_flow
{

    bool Flow::calculateDynamicPressure()
    {
        if (m_rho && m_velocity)
        {
            m_dynamicPressure = m_rho*m_velocity*m_velocity/2.0;
            return true;
        }
        return false;
    }

    bool Flow::calculateFlow()
     {
        if (m_mach && m_T0)
        {
            m_velocity = 20.04*m_mach*sqrt((m_T0 + T0_KELVIN)/(1.0 + 0.2*m_mach*m_mach));

            return calculateDynamicPressure();
        }
        return false;
    }


    void Flow::print()
    {
        std::cout << "Flow:\n"
            << "\trho = " << m_rho << "\n"
            << "\tvelocity = " << m_velocity << "\n"
            << "\tT0 = " << m_T0 << "\n"
            << "\tdynamicPressure  = " << m_dynamicPressure  << "\n"
            << "\tmach = " << m_mach << "\n"
            << "\treynolds = " << m_reynolds << "\n";
    }


    bool Flow::loadFile( const std::string& fileName)
    {
        std::ifstream fin(fileName);

        if (!fin.is_open())
        {
            std::cerr << "flow: can't open file " << fileName << "\n";
            return false;
        }

        std::cout << "Parsing file " << fileName << "...\n";
        std::string buff_s;

        // mach = *mach* T0  = *T0* rho = *rho*
        fin
            >> buff_s >>  buff_s >> m_mach
            >> buff_s >>  buff_s >> m_T0
            >> buff_s >>  buff_s >> m_rho;

        fin.close();

        return true;
    }


    /// todo вынести в сторонюю библиотеку парс трубы Т-313
    namespace
    {
        /**
        *     todo  разбивать строку из данных элементов
        */
        enum ptlTable
        {
                N_smth_Tx_POb_PC_M_AP_AI_P0_M_pkt_ls_t1_t2 = 1,
                N_Re_Q_t1_t2

        };

        /**
        *   Содержит строку с описанием таблицы (по ней определяется начало данных таблицы)
        */
        bool parsedContains(const std::string& str, const int table)
        {
            //std::cout << "parsedContains(): string = " << str << ", table = " << table << "\n";
            /**
            *   make special object of each table to perform better parsing
            *   ex: table discription, tavble, ended, tableDtat(data1, data2, etc...)
            */
            if (N_smth_Tx_POb_PC_M_AP_AI_P0_M_pkt_ls_t1_t2 == table)
            return (
                    (str.find("N")  != std::string::npos) &&
                    //(str.find("ñèíõð") != std::string::npos) &&
                    (str.find("TX")  != std::string::npos) &&
                    (str.find("P0b")  != std::string::npos) &&
                    (str.find("PC_M") != std::string::npos) &&
                    (str.find("AP") != std::string::npos) &&
                    (str.find("AI") != std::string::npos) &&
                    (str.find("P0_M") != std::string::npos) &&
                    (str.find("pkt_ls") != std::string::npos) &&
                    (str.find("t1") != std::string::npos) &&
                    //(str.find("ñåê") != std::string::npos) &&
                    (str.find("t2") != std::string::npos)
                    //(str.find("ñåê") != std::string::npos)
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


    /**
    *   Parse .ptl file (T-313 wt protocol)
    */
    bool Flow::parsePTLfile(const std::string& fileName)
    {
        std::cout << "parsePTLfile:: started\n";
        std::ifstream fin(fileName);

        if (!fin.is_open())
        {
            std::cerr << "Can't open file! aborting\n";
            return false;
        }
        std::cout << "parsePTLfile:: file " << fileName << " is opened\n";


        std::string buff_s;
        double buff_d;
        int counter = 1;
        while (!parsedContains(buff_s, N_smth_Tx_POb_PC_M_AP_AI_P0_M_pkt_ls_t1_t2))
        {
            std::getline(fin, buff_s);
            counter++ ;
        }
        /// нашли данные таблицы с отметками времени сбора данных колебаний модели
        std::cout << "FOUND TABLE of isAnglePolling\n";


        int isAnglePolling = 2; //2 no, 3 yes
        int buff_i;
        fin >> buff_i; // N
        while ( isAnglePolling != 3 && buff_s != "______________________________________________________________________________________")
        {
            fin >> isAnglePolling;

            for(int i = 0; i < 8; i++)
                fin >> buff_s;

            fin >> buff_d; // t2
            fin >> buff_s; // N of the next string

            counter++ ;
        }

        std::cout << "FOUND DATA of isAnglePolling\n";

        std::vector<double> timeAnglePolling;
        timeAnglePolling.push_back(buff_d);

        while ( isAnglePolling == 3 && buff_s != "______________________________________________________________________________________")
        {
            fin >> isAnglePolling;

            for(int i = 0; i < 8; i++)
                fin >> buff_s;


            fin >> buff_d; // t2
            timeAnglePolling.push_back(buff_d);

            fin >> buff_s; // N of the next string

            counter++ ;
        }


        while (!parsedContains(buff_s, N_Re_Q_t1_t2))
        {
            std::getline(fin, buff_s);
            counter++;
        }


        std::vector<double> Re;
        std::vector<double> q;
        std::vector<double> t;

            /**
            *   TODO
            *   make special object
            *   make connection to *name*_flow file -> find flow parameters during test period
            */

        do
        {
            fin >> buff_s  // N
                >> buff_d; // Re
                Re.push_back(buff_d);
            fin >> buff_d;
                q.push_back(buff_d);

            fin >> buff_d; // t1
                t.push_back(buff_d);

            fin >> buff_s; // t2

        } while( buff_s != "______________________________________________________________________________________");

        fin.close();


        std::ofstream fout(fileName + "_parsed");
        for(size_t i = 0; i < Re.size(); i++)
        {
            // только часть относящаяся к эксперименту
            if (t.at(i) > timeAnglePolling.at(0) && t.at(i) < timeAnglePolling.back())
            {
                fout
                    << t.at(i)  << "\t"
                    << Re.at(i) << "\t"
                    << q.at(i)  << "\n";
            }
        }
        fout.close();

        return true;
    }


} // namespace wt_flow
