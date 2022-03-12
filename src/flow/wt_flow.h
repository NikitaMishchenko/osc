#pragma once

#include <math.h>


namespace wt_flow
{
    const double T0_KELVIN = 273.15;

    class Flow
    {
    private:
        double m_rho = 0;
        double m_velocity = 0;
        double m_T0 = 0;

        double m_dynamicPressure = 0;

        // reference
        double m_mach = 0;
        double m_reynolds = 0; // normalized at 1 meter


    public:
        ///CALCULATE FLOW
        void setToDefault()
        {
            m_velocity = 1;
            m_dynamicPressure = 1;
        }

        bool calculateDynamicPressure()
        {
            if (m_rho && m_velocity)
            {
                m_dynamicPressure = m_rho*m_velocity*m_velocity/2.0;
                return true;
            }
            else
            {
                return false;
            }
        }

        bool calculateFlow()
        {
            if (m_mach && m_T0)
            {
                m_velocity = 20.04*m_mach*sqrt((m_T0 + T0_KELVIN)/(1.0 + 0.2*m_mach*m_mach));

                if (!calculateDynamicPressure())
                    return true;
                else
                    return false;
            }
            else
            {
                return false;
            }
        }


        bool isCalculated()
        {
            return m_dynamicPressure && m_velocity;
        }


        double getDynamicPressure()
        {
            return m_dynamicPressure;
        }



        // todo remove
        double calculateVelocityFormula(const double& mach, const double& T0for )
        {
            return 20.04*mach*sqrt((T0for + T0_KELVIN)/(1.0 + 0.2*mach*mach));
        }


        /// IO

        void print()
        {
            std::cout
            << "Flow:\n"
                << "\trho = " << m_rho << "\n"
                << "\tvelocity = " << m_velocity << "\n"
                << "\tT0 = " << m_T0 << "\n"
                << "\tdynamicPressure  = " << m_dynamicPressure  << "\n"
                << "\tmach = " << m_mach << "\n"
                << "\treynolds = " << m_reynolds << "\n";
        }

        bool loadFile( const std::string& fileName)
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

        //namespace
        //{
            bool parsedContains(const std::string& str)
            {
                return (
                        (str.find("N")  != std::string::npos) &&
                        (str.find("Re") != std::string::npos) &&
                        (str.find("Q")  != std::string::npos) &&
                        (str.find("N")  != std::string::npos) &&
                        (str.find("t1") != std::string::npos) &&
                        (str.find("t1") != std::string::npos)
                        );
            }
        //} // namespace

        bool parsePTLfile(const std::string& fileName)
        {
            std::cout << "parsePTLfile:: started\n";
            std::ifstream fin(fileName);

            std::cin.get();

            if (!fin.is_open())
            {
                std::cerr << "Can't open file! aborting\n";
                return false;
            }

            std::cout << "parsePTLfile:: file " << fileName << " is opened\n";
            std::cin.get();

            std::string buff_s;

            int counter = 1;
            while (!parsedContains(buff_s))
            {
                std::getline(fin, buff_s);
                std::cout << "stringNo = " << counter++ << "\n";
                std::cout << buff_s << "\n";
                std::cin.get();
            }

            std::cout << "STOPED\n";
            std::cin.get();

            std::vector<double> Re;
            std::vector<double> q;
            double buff_d;

            /**
            *   TODO
            *   save time
            *   make special object
            *   make connection to *name*_flow file -> find flow parameters during test period
            */

            do
            {
                fin >> buff_s >> buff_d;
                    Re.push_back(buff_d);
                fin >> buff_d;
                    q.push_back(buff_d);
                fin >> buff_s >> buff_s;

            }
            while( buff_s != "______________________________________________________________________________________");

            fin.close();

            std::ofstream fout(fileName + "_parsed");
            for(size_t i = 0; i < Re.size(); i++)
            {
                std::cout << Re.at(i) << "\t" << q.at(i) << "\n";
                fout << Re.at(i) << "\t" << q.at(i) << "\n";
            }
            fout.close();

            return true;
        }

    };

} // namespace wt_flow
