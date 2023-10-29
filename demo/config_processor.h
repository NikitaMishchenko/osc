#pragma once

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

struct DataToProc
{
    DataToProc(const std::string &coreName,
               const std::string &modelName,
               const boost::filesystem::path &basePath)
        : m_coreName(coreName),
          m_modelName(modelName),
          m_basePath(basePath)
    {
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "DataToProc:\n"
           << "coreName = " << m_coreName << "\n"
           << "modelName = " << m_modelName << "\n"
           << "basePath = " << m_basePath << "\n";

        return ss.str();
    }

    std::string m_coreName;
    std::string m_modelName;
    boost::filesystem::path m_basePath;
};

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

        m_root = tree.get<std::string>("description.pathToData");

        std::cout << "Root of data to proceed: " << m_root << "\n";

        std::vector<std::string> fruits;
        for (const auto& data : tree.get_child("description.dataToProceed"))
        {
            m_dataToProc.push_back(DataToProc(data.second.get<std::string>("coreName"),
                                              data.second.get<std::string>("modelName"),
                                              m_root));
        }
    }

    ~ConfigProcessor() {}

    std::vector<DataToProc> getDataToProc() const
    {
        return m_dataToProc;
    }

private:
    boost::filesystem::path m_root;
    std::vector<DataToProc> m_dataToProc;
};