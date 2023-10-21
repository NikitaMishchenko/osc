#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"

#include "analize_coefficients/specific/amplitude/utils.h"
#include "analize_coefficients/dynamic_coefficients_section.h"

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

    ProcessorInput processorInput(basePath, coreName, modelName);

    bool dataLoadedOk = false;
    std::string descriptionLoadData;
    Oscillation oscillation;
    wt_flow::Flow flow;
    Model model;

    std::tie(dataLoadedOk, descriptionLoadData, oscillation, flow, model) = processorInput.loadInputData();
    {
        if (!dataLoadedOk)
        {
            std::cerr << "failed to load input data!";
            throw;
        }

        descriptionStream << descriptionLoadData << std::endl;
    }

    boost::filesystem::path outputDataPath = basePath.string() + coreName;

    WtOscillation wtOscillation(oscillation, flow, model);

    ProcessorOutput processorOutput(basePath, coreName);

    bool dataWrittenOk = false;
    std::string descriptionWriteData;

    std::tie(dataWrittenOk, descriptionWriteData) = processorOutput.write(wtOscillation);
    descriptionStream << descriptionWriteData;

    std::string specificWtOscFile = coreName + ".wt_oscillation";

    ///
    //***********************************************************************************************
    ///

    const double maxAngle = *std::max_element(wtOscillation.angleBegin(), wtOscillation.angleEnd());
    const double minAngle = *std::min_element(wtOscillation.angleBegin(), wtOscillation.angleEnd());

    descriptionStream << "Максимальный достигаемый угол: " << maxAngle
                      << std::endl
                      << "Минимально достигаемый угол: " << minAngle
                      << std::endl;

    bool sectionEnabled = true;
    std::string sectionFilesGnuplotFile;

    if (sectionEnabled)
    {
        const int sectionBorderValue = int(std::min(std::abs(maxAngle), std::abs(minAngle)) / 5) * 5;

        descriptionStream << "Сечения в промежутке [" << -sectionBorderValue << ", " << sectionBorderValue << "]"
                          << std::endl;

        const double sectionAngleStep = 5;
        int countOfIteration = 0;
        for (int sectionAngle = -sectionBorderValue; sectionAngle <= sectionBorderValue;)
        {
            descriptionStream << "Рассчет методом сечений для угла " << sectionAngle << " градусов\n";
            std::cout << "sectionAngle = " << sectionAngle << "\n";

            std::string graphDecoration = ("using 4:3 pt " + std::to_string(countOfIteration) + " lc " + std::to_string(countOfIteration));

            bool isOk = false;
            Oscillation section;

            std::tie(isOk, section) = makeSection(oscillation, sectionAngle, ASCENDING);

            {
                std::string specificSectionFile = coreName + "_section" + std::to_string(sectionAngle) + "_asc.oscillation";
                sectionFilesGnuplotFile += ", \"" + specificSectionFile + "\" " + graphDecoration;

                descriptionStream << "Сохранение данных сечения в файл: "
                                  << "\"" << specificSectionFile << "\""
                                  << std::endl;

                {
                    std::ofstream fout(boost::filesystem::path(outputDataPath / specificSectionFile).string());

                    fout << section << "\n";
                }
            }

            std::tie(isOk, section) = makeSection(oscillation, sectionAngle, DESCENDING);

            {
                std::string specificSectionFile = coreName + "_section" + std::to_string(sectionAngle) + "_desc.oscillation";
                sectionFilesGnuplotFile += ", \"" + specificSectionFile + "\" " + graphDecoration;

                descriptionStream << "Сохранение данных сечения в файл: "
                                  << "\"" << specificSectionFile << "\""
                                  << std::endl;

                {
                    std::ofstream fout(boost::filesystem::path(outputDataPath / specificSectionFile).string());

                    fout << section << "\n";
                }
            }

            sectionAngle += sectionAngleStep;
            countOfIteration++;
        }
    }
    descriptionStream << "Построить график a''(a'):\n"
                      << "plot \"" << specificWtOscFile << "\" using 4:3 with linespoints"
                      << (sectionFilesGnuplotFile.empty()
                              ? ""
                              : sectionFilesGnuplotFile)
                      << std::endl;

    descriptionStream << "Построить график mz(a):\n"
                      << "plot \"" << specificWtOscFile << "\" using 2:($4*" << wtOscillation.getMzNondimensionalization() << ") with lines"
                      << std::endl;

    ///
    //***********************************************************************************************
    ///

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
    //{DataToProc(4465, "shpereCone2.model")};

    std::string buff;
    for (auto d : dataToProc)
    {
        doJob(std::to_string(d.dataIndex), d.modelName);
    }
}
