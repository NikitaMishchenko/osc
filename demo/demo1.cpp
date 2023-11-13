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

    std::shared_ptr<DescriptionStream> descriptionStreamPtr = std::make_shared<DescriptionStream>(dataToProc.m_basePath / coreName, coreName + ".description");
    std::shared_ptr<DescriptionStream> gnuplotGraphStreamPtr = std::make_shared<DescriptionStream>(dataToProc.m_basePath / coreName, coreName + "_gnuplot_graph" + ".description");
    
    ///
    //***********************************************************************************************
    ///

    *descriptionStreamPtr << "Определяем корневое имя файла с описанием работы с данными: "
                      << "\"" << coreName << "\""
                      << std::endl;

    Oscillation oscillation;
    wt_flow::Flow flow;
    Model model;
    {
        ProcessorInput processorInput(descriptionStreamPtr, basePath, coreName, modelName);

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
        *descriptionStreamPtr << "Рассчет методом сечений для угла " << sectionAngleStep << " градусов\n";
        
        // Sections sections(oscillation, sectionAngleStep);
        // sections.calculate();

        ProcessorOutput processorOutput(descriptionStreamPtr, gnuplotGraphStreamPtr, summaryStreamPtr, basePath, coreName);

        bool dataWrittenOk = false;

        dataWrittenOk = processorOutput.write(wtOscillation);
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
    {
        boost::filesystem::path configPath = "";
        configProcessor.load(configPath.string());
    }
    
    boost::filesystem::path rootDataPath = configProcessor.getRootPath();
    std::vector<DataToProc> dataToProc = configProcessor.getDataToProc();
    
    std::shared_ptr<DescriptionStream> summaryStreamPtr = std::make_shared<DescriptionStream>(rootDataPath, "summary");

    for (const auto& d : dataToProc)
    {
        std::cout << d.toString() << "\n";
        doJob(d, summaryStreamPtr);
    }

    DescriptionStream summary(rootDataPath, "summary.description");

    {
        std::stringstream streamGnuplotGraph; 

        streamGnuplotGraph << gnuplot_scripts::amplitudeSummary(summaryStreamPtr->getDescriptionFileName())
                           << std::endl;
        
        const std::string AmplitudeNondimGnuplotFileName = "amplitude_to_w_nondim.gp";

        boost::filesystem::path gnuplotAmplitudeNondimGnuplotFile = rootDataPath / "plotters" / AmplitudeNondimGnuplotFileName;

        {
            std::ofstream fout(gnuplotAmplitudeNondimGnuplotFile.string());
        
            fout << "reset\n"
                 << streamGnuplotGraph.str() << std::endl;
        }

        summary << "Построить график амплитуды от безразмерной частоты (coreName, amplitudeAngle[degre], avgW [rad/s], nondimW):\n"
                << streamGnuplotGraph.str() << std::endl;
    }

    
    // todo ? make single Class for description and data stream getDataStream() << , detDescriptionStream() << , getFileNameDescirption(), getFileNameData()
    // struct description, data -> binds data and it's description
    // description passed for other sources, data
}
