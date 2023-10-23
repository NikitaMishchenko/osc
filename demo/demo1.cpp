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

class DescriptionWriter
{
public:
    DescriptionWriter(boost::filesystem::path path, std::string fileName = "") : m_descriptionName(fileName)
    {
        if (!m_descriptionName.empty())
            m_descriptionName += ".description";
        else
            m_descriptionName = "description";

        m_descriptionFile = path / m_descriptionName;
    }

    ~DescriptionWriter()
    {
        std::ofstream fout(m_descriptionFile.string());

        fout << m_descriptionStream.str();
    }

private:
    std::string m_descriptionName;
    boost::filesystem::path m_descriptionFile;

    std::stringstream m_descriptionStream;
    std::ofstream fout;
};

void doJob(const std::string &coreName, const std::string &modelName)
{
    boost::filesystem::path basePath = "/home/mishnic/data/data_proc/sphere_cone_M1.75/";

    ///
    //***********************************************************************************************
    ///
    std::stringstream descriptionStream;

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
    amplitude::AngleAmplitude amplitude(std::make_shared<std::vector<double>>(wtOscillation.getTime()),
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

    ///
    //***********************************************************************************************
    ///

    {
        std::string descriptionFileName = coreName + ".description";

        std::cout << "Сохранение информации по обработке данных в файл: "
                  << "\"" << outputDataPath.string() + "/" + descriptionFileName << "\""
                  << std::endl;

        std::ofstream fout(outputDataPath.string() + "/" + descriptionFileName);

        fout << descriptionStream.str() << "\n";
    }

    // /home/mishnic/data/phd/data_proc/pic_ddangle_respect_to_angle_of_attack

    ///
    //***********************************************************************************************
    ///
}

struct DataToProc
{
    DataToProc(int dI, std::string mN) : dataIndex(dI), modelName(mN)
    {
    }

    int dataIndex;
    std::string modelName;
};

void rewriteData(const DataToProc &d)
{
    std::ifstream fin("/home/mishnic/data/phd/data_proc/4461..4474/data_to_filter/" + std::to_string(d.dataIndex));

    std::ofstream fout("/home/mishnic/data/phd/sphere_cone_M1.75/" + std::to_string(d.dataIndex) + "/" + std::to_string(d.dataIndex) + ".angle_history");

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
    std::vector<DataToProc> dataToProc =
        {
            DataToProc(4463, "shpereCone1.model"),
            DataToProc(4470, "shpereCone1.model"),
            DataToProc(4474, "shpereCone1.model"),
            DataToProc(4465, "shpereCone2.model"),
            DataToProc(4468, "shpereCone2.model"),
            DataToProc(4471, "shpereCone2.model"),
            DataToProc(4472, "shpereCone2.model")};

    std::string buff;
    for (auto d : dataToProc)
    {
        doJob(std::to_string(d.dataIndex), d.modelName);
    }
}
