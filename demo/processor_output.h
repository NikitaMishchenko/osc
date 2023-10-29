#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <numeric>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"
#include "analize_coefficients/specific/section/section.h"

#include "processor_io.h"

class ProcessorOutput : public ProcessorIo
{
public:
    ProcessorOutput(std::stringstream &descriptionStream,
                    const boost::filesystem::path &outputPath,
                    const std::string &coreName) : ProcessorIo(descriptionStream),
                                                   m_coreName(coreName),
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
        m_descriptionStream << "Сохранение данных колебаний в файл: "
                            << m_wtOscillationFile
                            << std::endl;

        {
            std::ofstream fout(m_wtOscillationFile.string());

            fout << wtOscillation << "\n";
        }

        m_descriptionStream << "Средняя частота колебаний w[Гц] = "
                            << wtOscillation.getW()
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
                            << "plot " << m_wtOscillationFile << " using 2:($4*" << wtOscillation.getMzNondimensionalization() << ") with lines"
                            << std::endl;
    }

    void reportAmplitude(const WtOscillation &wtOscillation) const
    {
        m_descriptionStream << "#######################################################\n";

        m_descriptionStream << "Сохранение данных амплитуд в файл: "
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

        m_descriptionStream << "Сохранение данных абсолютной амплитуды в файл: "
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

            m_descriptionStream << "Предельная средняя амплитуда автоколебаний: "
                                << angleAmplitudeAvg.m_amplitudeAngle
                                << std::endl;

            m_descriptionStream << "Средняя частота автоколебаний: "
                                << angleAmplitudeAvg.m_frequency
                                << std::endl;     

            m_descriptionStream << "Безразмерная частота колебаний для участка автоколебаний [Гц]: "
                                << calcWzNondimentional(angleAmplitudeAvg.m_frequency,
                                                        wtOscillation.getFlow(),
                                                        wtOscillation.getModel())
                                << std::endl;

            m_descriptionStream << "Построить график амплитуды и предельной амплитуды:\n"
                                << "plot "
                                << m_wtOscillationFile << " using 1:2 with lines, "
                                << m_angleHistroyAbsAmplitudeFile << " using 1:2, "
                                << angleAmplitudeAvg.m_amplitudeAngle << " lw 3 lc rgb \"red\""
                                << std::endl;
        }
    }

    void reportSections(const WtOscillation &wtOscillation,
                        const Sections &sections) const
    {
        bool isOk = false;
        std::vector<Section> sectionVector;
        const int sectionAngleStep = 5;
        double maxAngle, minAngle;
        std::tie(isOk, minAngle, maxAngle, sectionVector) = sections.getData();

        m_descriptionStream << "Максимальный достигаемый угол: " << maxAngle
                            << std::endl
                            << "Минимально достигаемый угол: " << minAngle
                            << std::endl;

        m_descriptionStream << "Построем сечения по углам с шагом " << sectionAngleStep
                            << std::endl;

        {
            std::string sectionFilesGnuplotFile;
            int sectionNo = 0;
            for (const auto &section : sectionVector)
            {
                std::string specificSectionFile = m_coreName + "_section" + std::to_string(section.getTargetAngle()) + "_" + std::string(Section::ASCENDING == section.getSectionType() ? "asc" : "desc") + ".oscillation";

                std::string graphDecoration = ("using 4:3 pt " + std::to_string(sectionNo) + " lc " + std::to_string(sectionNo));
                sectionFilesGnuplotFile += ", \"" + specificSectionFile + "\" " + graphDecoration;

                m_descriptionStream << "Сохранение данных сечения в файл: "
                                    << "\"" << specificSectionFile << "\""
                                    << std::endl;

                {
                    std::ofstream fout(boost::filesystem::path(m_outputPath / specificSectionFile).string());

                    fout << section << "\n";
                }

                sectionNo++;
            }

            m_descriptionStream << "Построить график a''(a'):\n"
                                << "plot " << m_wtOscillationFile << " using 4:3 with linespoints"
                                << (sectionFilesGnuplotFile.empty()
                                        ? ""
                                        : sectionFilesGnuplotFile)
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

    boost::filesystem::path m_outputPath;
};