#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <numeric>
#include <memory>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"
#include "analize_coefficients/specific/section/section.h"

#include "gnuplot_script_helper.h"
#include "processor_io.h"


/**
 * Manage data processing result and describes procedures performed,
 * saves data calculated
*/
class ProcessorOutput : public ProcessorIo
{
public:
    ProcessorOutput(std::stringstream &descriptionStream,
                    std::shared_ptr<std::stringstream> summaryStream,
                    const boost::filesystem::path &outputPath,
                    const std::string &coreName) : ProcessorIo(descriptionStream),
                                                   m_coreName(coreName),
                                                   m_summaryStream(summaryStream),
                                                   m_outputPath(outputPath)
    {
        // working in folder of specific core data
        m_descriptionStream << "Начинаем записывать данные. Исходный путь: " << m_outputPath << std::endl;
        m_outputPath /= coreName;
        m_descriptionStream << "Рабочая дирректория для записи данных: " << m_outputPath << std::endl;

        m_wtOscillationName = coreName + ".wt_oscillation";
        m_amplitudeName = coreName + "_amplitude.angle_history";
        m_absAmplitudeName = coreName + "_abs_amplitude.angle_history";

        m_wtOscillationFile = m_outputPath / m_wtOscillationName;
        m_angleHistroyAmplitudeFile = m_outputPath / m_amplitudeName;
        m_angleHistroyAbsAmplitudeFile = m_outputPath / m_absAmplitudeName;
    }

    virtual ~ProcessorOutput()
    {
        *m_summaryStream << std::endl;
        m_descriptionStream << "Запись данных закончена!" << std::endl;
    }

    bool write(const WtOscillation &wtOscillation,
               const Sections &sections)
    {
        reportWtOscillation(wtOscillation);
        reportAmplitude(wtOscillation);
        reportSections(wtOscillation, sections);

        return true;
    }

protected:
    void reportWtOscillation(const WtOscillation &wtOscillation) const
    {
        m_descriptionStream << "Сохранение данных колебаний в файл (время, первая, вторая, третья производые по времени): "
                            << m_wtOscillationFile
                            << std::endl;

        {
            std::ofstream fout(m_wtOscillationFile.string());

            fout << wtOscillation << "\n";
        }

        m_descriptionStream << "Средняя частота колебаний f[Гц] = "
                            << wtOscillation.getW()/2/M_PI
                            << std::endl;

        m_descriptionStream << "Средняя круговая частота колебаний w = "
                            << wtOscillation.getW()
                            << std::endl;

        m_descriptionStream << "Начальный угол колебаний [a_0] = "
                            << wtOscillation.getAngle(0)
                            << std::endl;

        m_descriptionStream << "Коэффициент обезразмеривания для получения mz = I/(qsl)a'' -> I/(qsl) = "
                            << wtOscillation.getMzNondimensionalization()
                            << std::endl;

        m_descriptionStream << "Безразмерный момент инерции iz = 2I/(rho*s*l) = "
                            << wtOscillation.getIzNondimensional()
                            << std::endl;

        m_descriptionStream << "Безразмерная частота колебаний w = w_avt*l/v = "
                            << wtOscillation.getWzNondimentional()
                            << std::endl;

        m_descriptionStream << "Построить график mz(a):\n"
                            << gnuplot_scripts::mz(m_wtOscillationFile, wtOscillation.getMzNondimensionalization())
                            << std::endl;

        *m_summaryStream << m_coreName << " ";
    }

    void reportAmplitude(const WtOscillation &wtOscillation) const
    {
        m_descriptionStream << "#######################################################\n";

        m_descriptionStream << "Сохранение данных амплитуд в файл"
                               "(время, первая, вторая производые по времени, частота, индекс отсчета в исходных данных): "
                            << m_angleHistroyAmplitudeFile
                            << std::endl;

        amplitude::AngleAmplitudeVector amplitudeVector = wtOscillation.getAngleAmplitudeVector();

        {
            std::ofstream fout(m_angleHistroyAmplitudeFile.string());

            fout << amplitudeVector << "\n";
        }

        amplitude::AngleAmplitudeBase maxAmplitude;
        maxAmplitude = amplitudeVector.getMaxAmplitude();
        amplitude::AngleAmplitudeBase minAmplitude;
        minAmplitude = amplitudeVector.getMinAmplitude();

        m_descriptionStream << "Максимальная амплитуда колебаний: "
                            << maxAmplitude.m_amplitudeAngle
                            << std::endl
                            << "Минимальная амплитуда колебаний: "
                            << minAmplitude.m_amplitudeAngle
                            << std::endl;

        amplitudeVector.sortViaTime();

        m_descriptionStream << "Сохранение данных абсолютной амплитуды в файл"
                               "(время, первая, вторая производые по времени, частота, индекс отсчета в исходных данных): "
                            << m_angleHistroyAbsAmplitudeFile
                            << std::endl;
        {
            std::ofstream fout(m_angleHistroyAbsAmplitudeFile.string());

            fout << amplitudeVector;
        }

        // ANALYSIS OF AMPLITUDE

        // todo make formal precise creteria
        const bool isAmplitudeDecreasing = maxAmplitude.m_amplitudeTime < minAmplitude.m_amplitudeTime;
        const double extrenumAmplitude = (isAmplitudeDecreasing ? minAmplitude.m_amplitudeAngle : maxAmplitude.m_amplitudeAngle);
        m_descriptionStream << "Характер динамики амплитуды: "
                            << (isAmplitudeDecreasing ? "падение " : "рост ")
                            << "амплитуды до " << extrenumAmplitude
                            << std::endl;

        // todo most frequent amplitude

        const double limitAmplitudeRatio = 0.1; // hardcode

        m_descriptionStream << "Определение предельной амплитуды с допуском (avgAmpl[1-(ampl/extrnumAmpl) < ratio]): "
                            << limitAmplitudeRatio
                            << std::endl;

        {
            std::vector<amplitude::AngleAmplitudeBase> angleAmplitudeToAvg;
            angleAmplitudeToAvg.reserve(amplitudeVector.m_angleAmplitudeData.size());

            for (const auto &amplitude : amplitudeVector.m_angleAmplitudeData)
            {
                if (std::abs(1.0 - (std::abs(amplitude.m_amplitudeAngle / extrenumAmplitude))) < limitAmplitudeRatio)
                {
                    angleAmplitudeToAvg.push_back(amplitude);
                }
            }
            angleAmplitudeToAvg.shrink_to_fit();

            m_descriptionStream << "Первое значение амплитуды удовлетворяющее критерию (время, угол): "
                                << angleAmplitudeToAvg.front().m_amplitudeTime << ", "
                                << angleAmplitudeToAvg.front().m_amplitudeAngle << "\n"
                                << "Последнее значение амплитуды удовлетворяющее критерию (время, угол): "
                                << angleAmplitudeToAvg.back().m_amplitudeTime << ", "
                                << angleAmplitudeToAvg.back().m_amplitudeAngle << "\n";

            amplitude::AngleAmplitudeBase angleAmplitudeAvg = amplitude::getAvg(angleAmplitudeToAvg);

            m_descriptionStream << "Предельная средняя амплитуда автоколебаний [degree]: "
                                << angleAmplitudeAvg.m_amplitudeAngle
                                << std::endl;

            *m_summaryStream << angleAmplitudeAvg.m_amplitudeAngle << " ";

            m_descriptionStream << "Средняя частота автоколебаний w[рад/с]: "
                                << angleAmplitudeAvg.m_frequency
                                << std::endl;     

            *m_summaryStream << angleAmplitudeAvg.m_frequency << " ";


            const double wzMondimentional = calcWzNondimentional(angleAmplitudeAvg.m_frequency,
                                                        wtOscillation.getFlow(),
                                                        wtOscillation.getModel());

            m_descriptionStream << "Безразмерная частота колебаний для участка автоколебаний [1]: "
                                << wzMondimentional
                                << std::endl;

            *m_summaryStream << wzMondimentional << " ";

            m_descriptionStream << "Построить график амплитуды и предельной амплитуды:\n"
                                << gnuplot_scripts::amplitudeLimitAmplitude(m_wtOscillationFile,
                                                                            m_angleHistroyAbsAmplitudeFile,
                                                                            angleAmplitudeAvg.m_amplitudeAngle,
                                                                            m_coreName)
                                << std::endl;
        }
    }

    void reportSections(const WtOscillation &wtOscillation,
                        const Sections &sections) const
    {
        bool isOk = false;
        std::vector<Section> sectionVector;
        const int sectionAngleStep = 5; // hardcode
        double maxAngle, minAngle;
        std::tie(isOk, minAngle, maxAngle, sectionVector) = sections.getData();

        m_descriptionStream << "Максимальный достигаемый угол: " << maxAngle
                            << std::endl
                            << "Минимально достигаемый угол: " << minAngle
                            << std::endl;

        m_descriptionStream << "Построем сечения по углам с шагом " << sectionAngleStep
                            << std::endl;

        {
            std::stringstream sectionsGnuplotFileScript;
            std::vector<boost::filesystem::path> specificSectionFileVector;
            specificSectionFileVector.reserve(sectionVector.size());

            for (const auto &section : sectionVector)
            {
                std::string specificSectionFileName = m_coreName + "_section" + std::to_string(section.getTargetAngle()) + "_" + std::string(Section::ASCENDING == section.getSectionType() ? "asc" : "desc") + ".oscillation";
                boost::filesystem::path specificSectionFile = m_outputPath / specificSectionFileName;

                specificSectionFileVector.push_back(specificSectionFile);

                m_descriptionStream << "Сохранение данных сечения в файл (время, первая, вторая, третья производые по времени): "
                                    << specificSectionFile
                                    << std::endl;

                {
                    std::ofstream fout(boost::filesystem::path(m_outputPath / specificSectionFile).string());

                    fout << section << "\n";
                }
            }

            m_descriptionStream << "Построить график a''(a'):\n"
                                << gnuplot_scripts::amplitudeLimitAmplitude(m_wtOscillationFile,
                                                                            specificSectionFileVector,
                                                                            m_coreName)
                                << std::endl;
        }
    }

    boost::filesystem::path m_wtOscillationFile;
    boost::filesystem::path m_angleHistroyAmplitudeFile;
    boost::filesystem::path m_angleHistroyAbsAmplitudeFile;

private:
    std::string m_coreName;
    std::string m_wtOscillationName;
    std::string m_amplitudeName;
    std::string m_absAmplitudeName;

    std::shared_ptr<std::stringstream> m_summaryStream; // todo make it shared_pt

    boost::filesystem::path m_outputPath;
};