#pragma once

#include <vector>
#include <iostream>

#include <boost/program_options.hpp>


namespace po = boost::program_options;


class Options
{
public:
    Options()
    {
        descr.reset(new po::options_description("Allowed options"));

        descr->add_options()
            ("help,h", " - show basic info, help etc.")
            ("mode,m", po::value<std::string>()->default_value("none"),
             " - FFT to perform Fast Furue Transform procedures, P to perform periods, cut - to  cut realisation, All - to perform all procedures will be performed")
            ("file,f", po::value<std::string>()->default_value("file"), " - name of file")
            ("args,a", po::value<std::vector<double>>()->multitoken()->default_value(std::vector<double>{0}, ""), " - list of args");
    }


    bool parse_options(int ac, char* av[]);

    void show()
    {
        std::cout << *descr << std::endl;
    }

    std::string getMode()
    {
        return value<std::string>("mode");
    }

    std::string getFileName()
    {
        return value<std::string>("file");
    }

    std::vector<double> getArgs()
    {
        return value<std::vector<double>>("args");
    }

    bool exist(const char* name) const
    {
        return m_vm.count(name) != 0;
    }

private:
    po::variables_map m_vm;
    boost::shared_ptr<po::options_description> descr;

    template<typename T>
    T value(const char* name) const
    {
        return m_vm[name].as<T>();
    }

};
