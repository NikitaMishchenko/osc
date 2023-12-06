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
#include "codes.h"

namespace io
{
    /**
     * Manage data loading and describes procedures performed
     */
    class ProcessorInput : public ProcessorIo
    {
    public:
        ProcessorInput(std::shared_ptr<std::stringstream> descriptionStreamPtr,
                       const boost::filesystem::path &inputPath,
                       const std::string &coreName,
                       const std::string &modelName)
            : ProcessorIo({descriptionStreamPtr}),
              m_coreName(coreName),
              m_modelName(modelName),
              m_inputPathCore(inputPath)
        {
            // working in folder of specific core data
            m_inputPathProcessing = m_inputPathCore / coreName;

            m_angleHistoryName = m_coreName + ".angle_history";
            m_modelFile = m_modelName + ".model";
            m_flowName = m_coreName + ".flow";

            // m_descriptionName = coreName + ".description";

            m_angleHistoryFile = m_inputPathProcessing / m_angleHistoryName;
            m_modelFile = m_modelName;
            m_flowFile = m_inputPathCore / "flow_ptl" / m_flowName;

            // m_descriptionFile = m_inputPathProcessing / m_descriptionName;
        }

        virtual ~ProcessorInput()
        {
            // std::ofstream fout(m_descriptionFile.string());

            *getDescriptionStream() << "Загрузка данных закончена!" << std::endl;
        }

        /*
         * @returns errCode, oscillation data, flow data, model adata from files
         */
        std::tuple<int, Oscillation, wt_flow::Flow, Model>
        loadInputData(const double angleShift)
        {

            *getDescriptionStream() << "Определяем имя файла истории колебаний: "
                                    << m_angleHistoryFile
                                    << std::endl;

            AngleHistory angleHistory;
            {
                *getDescriptionStream() << "Загрузка истории угла атаки из файла: "
                                        << m_angleHistoryFile
                                        << std::endl;

                bool isOk = angleHistory.loadRaw(m_angleHistoryFile.string());

                if (angleShift)
                    angleHistory.codomainAdd(angleShift);

                if (!isOk)
                {
                    *getDescriptionStream() << std::string("failed to load AngleHistory from \"" + m_angleHistoryFile.string() + "\"") << std::endl;
                    return std::make_tuple(int(io::codes::FAILED_LOAD_ANGLE_HISTORY), Oscillation(), wt_flow::Flow(), Model());
                }
            }

            *getDescriptionStream() << "Расчет колебаний (Oscillation) по истории угла атаки (AngleHistory)"
                                    << std::endl;

            Oscillation oscillation(angleHistory);

            Model model;
            {
                *getDescriptionStream() << "Загрузка параметров модели из файла: "
                                        << m_modelFile
                                        << std::endl;

                bool isOk = model.loadFile(m_modelFile.string());

                if (!isOk)
                {
                    *getDescriptionStream() << std::string("failed to load Model from \"" + m_modelFile.string() + "\"") << std::endl;
                    return std::make_tuple(int(io::codes::FAILED_LOAD_MODEL), oscillation, wt_flow::Flow(), Model());
                }

                *getDescriptionStream() << "Параметры модели:\n"
                                        << model.getInfoString() << std::endl;
            }

            wt_flow::Flow flow;
            {
                *getDescriptionStream() << "Загрузка параметров потока из файла: "
                                        << "\"" << m_flowName << "\""
                                        << std::endl;

                bool isOk = flow.loadFile(m_flowFile.string());

                if (!isOk)
                {
                    *getDescriptionStream() << std::string("failed to load Flow from \"" + m_flowFile.string() + "\"") << std::endl;
                    return std::make_tuple(int(io::codes::FAILED_LOAD_MODEL), oscillation, wt_flow::Flow(), model);
                }

                *getDescriptionStream() << "Параметры потока:\n"
                                        << flow.getInfoString() << std::endl;
            }

            return std::make_tuple(int(io::codes::POSITIVE), oscillation, flow, model); // todo std::move
        }

        boost::filesystem::path getInputPath() const
        {
            return m_inputPathCore;
        }

    private:
        std::shared_ptr<std::stringstream> getDescriptionStream() { return m_descriptionStreamVector.at(0); }
        std::shared_ptr<std::stringstream> getGnuplotGraphStream() { return m_descriptionStreamVector.at(1); }

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

        boost::filesystem::path m_inputPathCore;
        boost::filesystem::path m_inputPathProcessing;
    };

} // nmespace io