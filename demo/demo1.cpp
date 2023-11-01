#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>


#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"

#include "analize_coefficients/specific/amplitude/utils.h"
#include "analize_coefficients/specific/section/section.h"

#include "io_descriptions/processor_input.h"
#include "io_descriptions/processor_output.h"
#include "io_descriptions/description_stream.h"
#include "io_descriptions/config_processor.h"

/**
 *  Файлы должны находиться в одном каталоге переменная root
 *  имена файлов
 *  core - префикс для всех файлов (например, номер протокола)
 *  .flow файл параметров потока,
 *  .model файл параметров модели
 *  .angle_history файл угол от времени
 *
 *   Выходные файлы
 *   .discription - описание проделанных процедур
 *
 */

void doJob(const DataToProc &dataToProc, std::shared_ptr<DescriptionStream> summaryStreamPtr)
{
    const std::string coreName = dataToProc.m_coreName;
    const std::string modelName = dataToProc.m_modelName;
    boost::filesystem::path basePath = dataToProc.m_basePath;

    DescriptionStream descriptionStream(dataToProc.m_basePath / coreName, coreName + ".description");
    
    ///
    //***********************************************************************************************
    ///

    descriptionStream << "Определяем корневое имя файла: "
                      << "\"" << coreName << "\""
                      << std::endl;

    Oscillation oscillation;
    wt_flow::Flow flow;
    Model model;
    {
        ProcessorInput processorInput(descriptionStream, basePath, coreName, modelName);

        bool dataLoadedOk = false;

        std::tie(dataLoadedOk, oscillation, flow, model) = processorInput.loadInputData(dataToProc.m_angleShift);

        if (!dataLoadedOk)
        {
            std::cerr << "failed to load input data!";
            throw;
        }
    }

    WtOscillation wtOscillation(oscillation, flow, model);
    {
        ///
        std::vector<Section> sectionVector;
        const int sectionAngleStep = 5;
        descriptionStream << "Рассчет методом сечений для угла " << sectionAngleStep << " градусов\n";
        Sections sections(oscillation, sectionAngleStep);
        sections.calculate();

        ProcessorOutput processorOutput(descriptionStream, summaryStreamPtr, basePath, coreName);

        bool dataWrittenOk = false;

        dataWrittenOk = processorOutput.write(wtOscillation, sections);
    }

    // /home/mishnic/data/phd/data_proc/pic_ddangle_respect_to_angle_of_attack
}

void rewriteData(const DataToProc &d)
{
    std::ifstream fin("/home/mishnic/data/phd/data_proc/4461..4474/data_to_filter/" + d.m_coreName);

    std::ofstream fout("/home/mishnic/data/phd/sphere_cone_M1.75/" + d.m_coreName + "/" + d.m_coreName + ".angle_history");

    std::string buff;

    while (!fin.eof())
    {
        fin >> buff;
        fout << buff << " ";
        fin >> buff;
        fout << buff << std::endl;
        fin >> buff >> buff;
    }
}

int main(int argc, char **argv)
{
    ConfigProcessor configProcessor;

    boost::filesystem::path configPath("/home/mishnic/data/data_proc/sphere_cone_M1.75/");
    configProcessor.load(configPath.string());

    std::vector<DataToProc> dataToProc = configProcessor.getDataToProc();
    
    std::shared_ptr<DescriptionStream> summaryStreamPtr = std::make_shared<DescriptionStream>(configPath, "summary");
    
    for (const auto& d : dataToProc)
    {
        std::cout << d.toString() << "\n";
        doJob(d, summaryStreamPtr);
    }

    DescriptionStream summary(configPath, "summary.description");

    summary << "Построить график амплитуды от безразмерной частоты:\n"
            << "plot " << summaryStreamPtr->getSescriptionFileName() << " using 2:4 lw 10"
            << std::endl;
}
