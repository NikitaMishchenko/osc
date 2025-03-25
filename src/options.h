#pragma once

#include <vector>
#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace options
{
    enum Procedure
    {
        CUT,
        FLOW,
        PERIODS,
        WT,
        PENDULUM,
        APPROXIMATION,
        COEFFICINETS_WINDOW,
        DYN_COEFFICINETS_WINDOW_WT_TEST,
        FILTER_GAUSS,
        TEST,
        UNKNOWN
    };

    namespace helpers
    {
        inline std::string toString(Procedure procedure)
        {
            switch(procedure)
            {
                case CUT:
                return std::string("CUT");

                case FLOW:
                return "FLOW";
                
                case PERIODS:
                return "PERIODS";

                case WT:
                return "WT";                

                case PENDULUM:
                return "PENDULUM";

                case APPROXIMATION:
                return "APPROXIMATION";

                case COEFFICINETS_WINDOW:
                return "COEFFICINETS_WINDOW";

                case DYN_COEFFICINETS_WINDOW_WT_TEST:
                return "DYN_COEFFICINETS_WINDOW_WT_TEST";           

                case FILTER_GAUSS:
                return "FILTER_GAUSS";

                case TEST:
                return "TEST"; 

                case UNKNOWN:
                return "UNKNOWN";                                 

                default:
                return "err: can't provide procedure name!";
            }
        }
    }
    
    class Options
    {
    public:
        Options()
        {
            descr.reset(new po::options_description("Base_Oscillation_Analisys build 23.04.22\nAllowed options"));

            descr->add_options()("help,h", " - show basic info, help etc.")("mode,m", po::value<std::string>()->default_value("none"),
                                                                            "P to perform periods, cut - to  cut realisation, pendullum - to perform p* osc analysis, approximation - to perform approx")
                                                                            ("file,f", po::value<std::string>()->default_value("file"), " - name of file")
                                                                            ("file2", po::value<std::string>()->default_value(""), " - name of file2")
                                                                            ("args,a", po::value<std::vector<double>>()->multitoken()->default_value(std::vector<double>(), ""), " - list of args");
        }

        bool parse_options(int ac, char *av[]);

        void show()
        {
            std::cout << *descr << std::endl;
        }

        void readme()
        {
            std::cout << "\nReadme\n"
                      << "\t-m Perods -f filename --file2 filename2 -args first_arg second_arg\n"
                      << "\t\tfilename raw file 1-st row - time, 2-nd row - angle\n"
                      << "\t\tfilename2 output file. time/ange/dange/ddangle\n"
                      << "\t\ttsingle_arg - shift of angle (could be ><=0)\n"
                      << "\t\tsecond_arg - number of halfperiods in file\n"
                      << "\n"

                      << "\t-m cut -f filename -args first_arg second_arg\n"
                      << "\t\tfilename raw file 1-st row - time, 2-nd row - angle\n"
                      << "\t\targs first_arg - cut from, second_arg - cut to\n"
                      << "\n"

                      << "\t\tfilename - file in oscillation-format\n"
                      << "\n";
        }

        std::string getMode()
        {
            return value<std::string>("mode");
        }

        Procedure getProcedure()
        {
            std::string mode(value<std::string>("mode"));

            if ("periods" == mode || "P" == mode)
                return Procedure::PERIODS;

            if ("cut" == mode)
                return Procedure::CUT;

            if ("WT" == mode)
                return Procedure::WT;

            if ("flow" == mode)
                return Procedure::FLOW;

            if ("pendulum" == mode)
                return Procedure::PENDULUM;

            if ("approximation" == mode || "ap" == mode)
                return Procedure::APPROXIMATION;

            if ("coeffWindow" == mode)
                return Procedure::COEFFICINETS_WINDOW;

            if ("dynCoeffWindowWtTest" == mode)
                return Procedure::DYN_COEFFICINETS_WINDOW_WT_TEST;

            if ("filterGauss" == mode)
                return Procedure::FILTER_GAUSS;

            // todo readme

            if ("test" == mode)
                return Procedure::TEST;

            return Procedure::UNKNOWN;
        }

        std::string getFileName()
        {
            return value<std::string>("file");
        }

        std::string getFileName2()
        {
            return value<std::string>("file2");
        }

        std::vector<double> getArgs()
        {
            return value<std::vector<double> >("args");
        }

        bool exist(const char *name) const
        {
            return m_vm.count(name) != 0;
        }

    private:
        po::variables_map m_vm;
        boost::shared_ptr<po::options_description> descr;

        template <typename T>
        T value(const char *name) const
        {
            return m_vm[name].as<T>();
        }
    };
}
