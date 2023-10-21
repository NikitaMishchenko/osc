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

void doJob(const std::string &coreName, const std::string &modelName)
{
    boost::filesystem::path basePath = "/home/mishnic/data/data_proc/sphere_cone_M1.75/";
    boost::filesystem::path root = basePath / coreName;
    std::stringstream descriptionStream;

    boost::filesystem::path fileToProceed = root;
    boost::filesystem::path workingPath = root;

    ///
    //***********************************************************************************************
    ///

    descriptionStream << "Определяем корневое имя файла: "
                      << "\"" << coreName << "\""
                      << std::endl;

    ///
    //***********************************************************************************************
    ///

    std::string specificInitialFile = coreName + ".angle_history";
    std::string descriptionFileName = coreName + ".discription";

    descriptionStream << "Определяем имя файла истории колебаний: "
                      << "\"" << specificInitialFile << "\""
                      << std::endl;

    descriptionStream << "Определяем имя файла описания процедуры обработки: "
                      << "\"" << descriptionFileName << "\""
                      << std::endl;

    ///
    //***********************************************************************************************
    ///

    descriptionStream << "Предварительный рассчет параметров колебаний для файла: "
                      << "\"" << specificInitialFile << "\""
                      << std::endl;

    Oscillation oscillation(
        AngleHistory(
            boost::filesystem::path(fileToProceed / specificInitialFile).string()));

    ///
    //***********************************************************************************************
    ///

    std::string modelFileName = modelName; //"shpereCone1.model";

    descriptionStream << "Загрузка параметров модели из файла: "
                      << "\"" << modelFileName << "\""
                      << std::endl;

    Model model;
    model.loadFile(boost::filesystem::path(root / modelFileName).string());

    descriptionStream << "Параметры модели:\n"
                      << model.getInfoString() << std::endl;

    ///
    //***********************************************************************************************
    ///

    std::string flowFileName = coreName + ".flow";

    descriptionStream << "Загрузка параметров потока из файла: "
                      << "\"" << flowFileName << "\""
                      << std::endl;

    wt_flow::Flow flow(boost::filesystem::path(workingPath / flowFileName).string());

    descriptionStream << "Параметры потока:\n"
                      << flow.getInfoString() << std::endl;

    ///
    //***********************************************************************************************
    ///

    WtOscillation wtOscillation(oscillation, flow, model);

    std::string specificWtOscFile = coreName + ".wt_oscillation";

    descriptionStream << "Сохранение данных колебаний в файл: "
                      << "\"" << specificWtOscFile << "\""
                      << std::endl;

    {
        std::ofstream fout(boost::filesystem::path(workingPath / specificWtOscFile).string());

        fout << wtOscillation << "\n";
    }

    descriptionStream << "Частота колебаний w[Гц] = "
                      << wtOscillation.getW()
                      << std::endl;

    descriptionStream << "Коэффициент обезразмеривания для получения mz = I/(qsl)a'' -> I/(qsl) = "
                      << wtOscillation.getMzNondimensionalization()
                      << std::endl;

    descriptionStream << "Безразмерный момент инерции iz = 2I/(rho*s*l) = "
                      << wtOscillation.getIzNondimensional()
                      << std::endl;

    descriptionStream << "Безразмерная частота колебаний w = w_avt*l/v = "
                      << wtOscillation.getWzNondimentional()
                      << std::endl;

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
                    std::ofstream fout(boost::filesystem::path(workingPath / specificSectionFile).string());

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
                    std::ofstream fout(boost::filesystem::path(workingPath / specificSectionFile).string());

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
        std::ofstream fout(boost::filesystem::path(workingPath / specificAmplitudeFile).string());

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

        std::ofstream fout(boost::filesystem::path(workingPath/specificAbsAmplitudeFile).string());

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
        std::cout << "Сохранение информации по обработке данных в файл: "
                  << "\"" << workingPath.string() +  "/"+ descriptionFileName << "\""
                  << std::endl;

        std::ofstream fout(workingPath.string() + "/" + descriptionFileName);

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
    // shpereCone1.model
    //{4463, 4470, 4474, 4465, 4468, 4471, 4472};

    /* {DataToProc(4463, "shpereCone1.model"),
    DataToProc(4470, "shpereCone1.model"),
    DataToProc(4474, "shpereCone1.model"),
    DataToProc(4465, "shpereCone2.model"),
    DataToProc(4468, "shpereCone2.model"),
    DataToProc(4471, "shpereCone2.model"),
    DataToProc(4472, "shpereCone2.model")}*/
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