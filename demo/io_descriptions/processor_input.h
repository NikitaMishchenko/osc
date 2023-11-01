#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"

#include "processor_io.h"

/**
 * Manage data loading and describes procedures performed
*/
class ProcessorInput : public ProcessorIo
{
public:
    ProcessorInput(std::stringstream& descriptionStream,
                   const boost::filesystem::path &inputPath,
                   const std::string &coreName,
                   const std::string &modelName)
        : ProcessorIo(descriptionStream),
          m_coreName(coreName),
          m_modelName(modelName), 
          m_inputPath(inputPath)
    {
        // working in folder of specific core data
        m_inputPath /= coreName;

        m_angleHistoryName = m_coreName + ".angle_history";
        m_modelFile = m_modelName + ".model";
        m_flowName = m_coreName + ".flow";

        // m_descriptionName = coreName + ".description";

        m_angleHistoryFile = m_inputPath / m_angleHistoryName;
        m_modelFile = m_inputPath / m_modelName;
        m_flowFile = m_inputPath / m_flowName;

        // m_descriptionFile = m_inputPath / m_descriptionName;
    }

    virtual ~ProcessorInput()
    {
        //std::ofstream fout(m_descriptionFile.string());

        m_descriptionStream << "Загрузка данных закончена!" << std::endl;
    }

    std::tuple<bool, Oscillation, wt_flow::Flow, Model>
    loadInputData(double angleShift) const
    {

        m_descriptionStream << "Определяем имя файла истории колебаний: "
                            << m_angleHistoryFile
                            << std::endl;

        AngleHistory angleHistory;
        {
            bool isOk = angleHistory.loadRaw(m_angleHistoryFile.string());

            if (angleShift)
                angleHistory.codomainAdd(angleShift);

            if (!isOk)
                throw std::runtime_error("failed to load AngleHistory from \"" + m_angleHistoryFile.string() + "\"");
        }

        Oscillation oscillation(angleHistory);

        m_descriptionStream << "Загрузка параметров модели из файла: "
                            << m_modelFile
                            << std::endl;

        Model model;
        {
            bool isOk = model.loadFile(m_modelFile.string());

            if (!isOk)
                throw std::runtime_error("failed to load Model from \"" + m_modelFile.string() + "\"");

            m_descriptionStream << "Параметры модели:\n"
                                << model.getInfoString() << std::endl;
        }

        m_descriptionStream << "Загрузка параметров потока из файла: "
                            << "\"" << m_flowName << "\""
                            << std::endl;

        wt_flow::Flow flow;
        {
            bool isOk = flow.loadFile(m_flowFile.string());

            if (!isOk)
                throw std::runtime_error("failed to load Flow from \"" + m_flowFile.string() + "\"");
        }

        m_descriptionStream << "Параметры потока:\n"
                            << flow.getInfoString() << std::endl;

        return std::make_tuple(true, oscillation, flow, model); // todo std::move
    }

    boost::filesystem::path getInputPath() const
    {
        return m_inputPath;
    }

protected:
    boost::filesystem::path m_angleHistoryFile;
    boost::filesystem::path m_modelFile;
    boost::filesystem::path m_flowFile;

    boost::filesystem::path m_descriptionFile;

    // std::string m_specificAngleHistoryFile;

private:
    std::string m_coreName;
    std::string m_angleHistoryName;
    std::string m_modelName;
    std::string m_flowName;

    boost::filesystem::path m_inputPath;
};
