#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>


#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"

#include "analize_coefficients/specific/amplitude/utils.h"
#include "analize_coefficients/specific/section/section.h"

#include "processor_input.h"
#include "processor_output.h"
#include "description_stream.h"
#include "config_processor.h"

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

void doJob(const DataToProc &dataToProc)
{
    const std::string coreName = dataToProc.m_coreName;
    const std::string modelName = dataToProc.m_modelName;
    boost::filesystem::path basePath = dataToProc.m_basePath;

    DescriptionStream descriptionStream(dataToProc.m_basePath / coreName, coreName);

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

        std::tie(dataLoadedOk, oscillation, flow, model) = processorInput.loadInputData();

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

        ProcessorOutput processorOutput(descriptionStream, basePath, coreName);

        bool dataWrittenOk = false;

        dataWrittenOk = processorOutput.write(wtOscillation, sections);
    }

    ///
    //***********************************************************************************************
    ///
    boost::filesystem::path outputDataPath = basePath.string() + coreName;

    std::string specificAmplitudeFile = coreName + ".amplitude";

    descriptionStream << "Сохранение данных амплитуды в файл: "
                      << "\"" << specificAmplitudeFile << "\""
                      << std::endl;
    amplitude::AngleAmplitudeVector amplitude(std::make_shared<std::vector<double>>(wtOscillation.getTime()),
                                              std::make_shared<std::vector<double>>(wtOscillation.getAngle()),
                                              std::make_shared<std::vector<double>>(wtOscillation.getDangle()));

    // AngleHistory amplitude(wtOscillation.getTimeAmplitude(), wtOscillation.getAngleAmplitude());

    {
        std::ofstream fout(boost::filesystem::path(outputDataPath / specificAmplitudeFile).string());

        fout << amplitude << "\n";
    }

    amplitude::AngleAmplitudeBase maxAmplitude;
    maxAmplitude = amplitude.getMaxAmplitude();
    amplitude::AngleAmplitudeBase minAmplitude;
    minAmplitude = amplitude.getMinAmplitude();

    descriptionStream << "Максимальная амплитуда колебаний: "
                      << maxAmplitude.m_amplitudeAngle
                      << std::endl
                      << "Минимальная амплитуда колебаний: "
                      << minAmplitude.m_amplitudeAngle
                      << std::endl;

    ///
    //***********************************************************************************************
    ///

    descriptionStream << "Рассчет амплитуды колебаний"
                      << std::endl;

    amplitude::AngleAmplitudeAnalyser angleAmplitudeAnalyser(amplitude);

    // todo
    // descriptionStream << "Самая часто встречающаяся амплитуда: "
    //                  << angleAmplitudeAnalyser.getMostFrequentAmplitudeValue()
    //                  << std::endl;

    {

        std::string specificAbsAmplitudeFile = coreName + "_abs.amplitude";

        descriptionStream << "Сохранение данных абсолютной амплитуды в файл: "
                          << "\"" << specificAbsAmplitudeFile << "\""
                          << std::endl;

        std::ofstream fout(boost::filesystem::path(outputDataPath / specificAbsAmplitudeFile).string());

        // std::vector<amplitude::AngleAmplitudeBase> sortedAmplitude = angleAmplitudeAnalyser.getSortedAmplitude();
        amplitude.sortViaTime();

        fout << amplitude;
        // for (auto sAmp : sortedAmplitude)
        //     fout << sAmp.m_amplitudeTime << "\t" << sAmp.m_amplitudeAngle << "\n";
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

    configProcessor.load("/home/mishnic/data/data_proc/sphere_cone_M1.75/");

    std::vector<DataToProc> dataToProc = configProcessor.getDataToProc();

    for (const auto& d : dataToProc)
    {
        std::cout << d.toString() << "\n";
        doJob(d);
    }
}
