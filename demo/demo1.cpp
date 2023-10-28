#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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

struct DataToProc
{
    DataToProc(const std::string &coreName,
               const std::string &modelName,
               const boost::filesystem::path &basePath)
        : m_coreName(coreName),
          m_modelName(modelName),
          m_basePath(basePath)
    {
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "DataToProc:\n"
           << "coreName = " << m_coreName << "\n"
           << "modelName = " << m_modelName << "\n"
           << "basePath = " << m_basePath << "\n";

        return ss.str();
    }

    std::string m_coreName;
    std::string m_modelName;
    boost::filesystem::path m_basePath;
};

class ConfigProcessor
{
public:
    ConfigProcessor()
    {
    }

    void load(const std::string &pathToConfig = "")
    {
        const std::string filename = pathToConfig + "config";
        boost::property_tree::ptree tree;
        boost::property_tree::read_json(filename, tree);

        m_root = tree.get<std::string>("description.pathToData");

        std::cout << "Root of data to proceed: " << m_root << "\n";

        std::vector<std::string> fruits;
        for (const auto& data : tree.get_child("description.dataToProceed"))
        {
            m_dataToProc.push_back(DataToProc(data.second.get<std::string>("coreName"),
                                              data.second.get<std::string>("modelName"),
                                              m_root));
        }
    }

    ~ConfigProcessor() {}

    std::vector<DataToProc> getDataToProc() const
    {
        return m_dataToProc;
    }

private:
    boost::filesystem::path m_root;
    std::vector<DataToProc> m_dataToProc;
};

class DescriptionStream : public std::stringstream
{
public:
    DescriptionStream(boost::filesystem::path path, std::string fileName = "") : m_descriptionName(fileName)
    {
        if (!m_descriptionName.empty())
            m_descriptionName += ".description";
        else
            m_descriptionName = "description";

        m_descriptionFile = path / m_descriptionName;
    }

    virtual ~DescriptionStream()
    {
        std::ofstream fout(m_descriptionFile.string());

        fout << this->str();
    }

private:
    std::string m_descriptionName;
    boost::filesystem::path m_descriptionFile;

    std::ofstream fout;
};

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
