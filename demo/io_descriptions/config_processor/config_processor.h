#pragma once

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "data_to_process.h"

class ConfigProcessor
{
public:
    ConfigProcessor()
    {
    }

    void load(const std::string &pathToConfig = "")
    {
        const std::string filename = pathToConfig + "config";
        boost::property_tree::ptree tree;
        boost::property_tree::read_json(filename, tree);

        m_root = tree.get<std::string>("description." + DataToProc::basePathKey);

        std::cout << "Root of data to proceed: " << m_root << "\n";

        std::vector<std::string> fruits;
        for (const auto& data : tree.get_child("description.dataToProceed"))
        {
            double sectionAngleStep;

            try
            {
                sectionAngleStep = data.second.get<double>(DataToOutput::sectionAngleStepKey);
            }
            catch(...)
            {
                sectionAngleStep = 5.0;
            }

            m_dataToProc.push_back(DataToProc(data.second.get<std::string>(DataToProc::coreNameKey),
                                              data.second.get<std::string>(DataToProc::modelNameKey),
                                              data.second.get<double>(DataToProc::angleShiftKey),
                                              m_root,
                                              DataToOutput(sectionAngleStep)));
        }
    }

    ~ConfigProcessor() {}

    std::vector<DataToProc> getDataToProc() const
    {
        return m_dataToProc;
    }

    boost::filesystem::path getRootPath()
    {   
        return m_root;
    }

public:
    const std::string extension = ".config";
private:
    boost::filesystem::path m_root;
    std::vector<DataToProc> m_dataToProc;
};