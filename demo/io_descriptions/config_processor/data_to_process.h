#pragma once

#include <string>
#include <sstream>

#include "boost/optional.hpp"
#include <boost/filesystem.hpp>

// todo make DataToProc combination of two structs DataInut(...) DataOutput (section)

struct DataToOutput
{
    DataToOutput(double sectionAngleStep) 
        : m_sectionAngleStep(sectionAngleStep)
    {}

    std::string toString() const
    {
        std::stringstream ss;
        ss << "DataToOutput:\n"
           << sectionAngleStepKey <<" = " << m_sectionAngleStep << "\n";

        return ss.str();
    }

    double m_sectionAngleStep;

    static const std::string dataToOutputKey;
    static const std::string sectionAngleStepKey;
};

struct DataToProc
{
    DataToProc(const std::string &coreName,
               const std::string &modelName,
               const double angleShift,
               const boost::filesystem::path &basePath,
               DataToOutput dataToOutput)
        : m_coreName(coreName),
          m_modelName(modelName),
          m_angleShift(angleShift),
          m_basePath(basePath),
          m_dataToOutput(dataToOutput)
    {
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << DataToProc::dataToProcKey << ":\n"
           << coreNameKey <<" = " << m_coreName << "\n"
           << modelNameKey << " = " << m_modelName << "\n"
           << angleShiftKey << " = " << m_angleShift << "\n"
           << basePathKey << " = " << m_basePath << "\n";

        ss << DataToOutput::dataToOutputKey << ":\n"; 

        return ss.str();
    }

    std::string m_coreName;
    std::string m_modelName;
    double m_angleShift; //< angle to shifth ".angle_history" file data
    boost::filesystem::path m_basePath; //< core path to data to process

    DataToOutput m_dataToOutput;

    public:

    static const std::string dataToProcKey;
    static const std::string coreNameKey;
    static const std::string modelNameKey;
    static const std::string angleShiftKey;
    static const std::string basePathKey;
};

