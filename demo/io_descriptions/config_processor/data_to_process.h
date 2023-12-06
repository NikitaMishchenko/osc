#pragma once

#include <string>
#include <sstream>

#include "boost/optional.hpp"
#include <boost/filesystem.hpp>

// todo make DataToProc combination of two structs DataInut(...) DataOutput (section)

struct DataToProc
{
    DataToProc(const std::string &coreName,
               const std::string &modelName,
               const double angleShift,
               const boost::optional<double> sectionAngleStep,
               const boost::filesystem::path &basePath)
        : m_coreName(coreName),
          m_modelName(modelName),
          m_angleShift(angleShift),
          m_sectionAngleStep(sectionAngleStep),
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
    double m_angleShift; //< angle to shifth ".angle_history" file data
    boost::optional<double> m_sectionAngleStep;
    boost::filesystem::path m_basePath; //< core path to data to process

    public:

    static const std::string coreNameKey;
    static const std::string modelNameKey;
    static const std::string angleShiftKey;
    static const std::string sectionAngleStepKey;
    static const std::string basePathKey;
};

