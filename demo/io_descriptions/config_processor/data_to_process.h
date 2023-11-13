#pragma once

#include <string>
#include <sstream>

#include <boost/filesystem.hpp>

struct DataToProc
{
    DataToProc(const std::string &coreName,
               const std::string &modelName,
               const double angleShift,
               const boost::filesystem::path &basePath)
        : m_coreName(coreName),
          m_modelName(modelName),
          m_angleShift(angleShift),
          m_basePath(basePath)
    {
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "DataToProc:\n"
           << coreNameKey <<" = " << m_coreName << "\n"
           << modelNameKey << " = " << m_modelName << "\n"
           << angleShiftKey << " = " << m_angleShift << "\n"
           << basePathKey << " = " << m_basePath << "\n";

        return ss.str();
    }

    std::string m_coreName;
    std::string m_modelName;
    double m_angleShift; //< angle to shifth ".angle_history" file
    boost::filesystem::path m_basePath; //< core path to data to process

    public:

    static const std::string coreNameKey;
    static const std::string modelNameKey;
    static const std::string angleShiftKey;
    static const std::string basePathKey;
};

const std::string DataToProc::coreNameKey  = "coreName";
const std::string DataToProc::modelNameKey = "modelName";
const std::string DataToProc::angleShiftKey = "angleShift";
const std::string DataToProc::basePathKey = "pathToData";