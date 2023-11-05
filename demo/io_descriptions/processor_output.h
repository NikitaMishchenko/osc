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
    ProcessorOutput(std::shared_ptr<std::stringstream> descriptionStreamPtr,
                    std::shared_ptr<std::stringstream> gnuplotGraphStreamPtr,
                    std::shared_ptr<std::stringstream> summaryStream,
                    const boost::filesystem::path &outputPath,
                    const std::string &coreName) : ProcessorIo({std::move(descriptionStreamPtr), std::move(gnuplotGraphStreamPtr)}),
                                                   m_coreName(coreName),
                                                   m_summaryStream(summaryStream),
                                                   m_outputPath(outputPath)
    {
        // working in folder of specific core data
        *getDescriptionStream() << "Начинаем записывать данные. Исходный путь: " << m_outputPath << std::endl;
        m_outputPath /= coreName;
        *getDescriptionStream() << "Рабочая дирректория для записи данных: " << m_outputPath << std::endl;

        m_wtOscillationName = coreName + ".wt_oscillation";
        m_amplitudeName = coreName + "_amplitude.angle_history";
        m_absAmplitudeName = coreName + "_abs_amplitude.angle_history";

        m_wtOscillationFile = m_outputPath / m_wtOscillationName;
        m_angleHistroyAmplitudeFile = m_outputPath / m_amplitudeName;
        m_angleHistroyAbsAmplitudeFile = m_outputPath / m_absAmplitudeName;

        m_periodsBaseDir = m_outputPath / "periods";
    }

    virtual ~ProcessorOutput()
    {
        *m_summaryStream << std::endl;
        *getDescriptionStream() << "Запись данных закончена!" << std::endl;
    }

    bool write(const WtOscillation &wtOscillation,
               const Sections &sections) const
    {
        reportWtOscillation(wtOscillation);
        reportAmplitude(wtOscillation);
        reportSections(wtOscillation, sections);
        reportPeriods(wtOscillation);

        return true;
    }

protected:
    void reportWtOscillation(const WtOscillation &wtOscillation) const
    {
        *getDescriptionStream() << "Сохранение данных колебаний в файл (время, первая, вторая, третья производые по времени): "
                                << m_wtOscillationFile
                                << std::endl;

        {
            std::ofstream fout(m_wtOscillationFile.string());

            fout << wtOscillation << "\n";
        }

        *getDescriptionStream() << "Средняя частота колебаний f[Гц] = "
                                << wtOscillation.getW() / 2 / M_PI
                                << std::endl;

        *getDescriptionStream() << "Средняя круговая частота колебаний w0 = "
                                << wtOscillation.getW()
                                << std::endl;

        *getDescriptionStream() << "Начальный угол колебаний [a_0] = "
                                << wtOscillation.getAngle(0)
                                << std::endl;

        *getDescriptionStream() << "Коэффициент обезразмеривания для получения mz = I/(qsl)a'' -> I/(qsl) = "
                                << wtOscillation.getMzNondimensionalization()
                                << std::endl;
        
        *getDescriptionStream() << "Коэффициент обезразмеривания для wz_nondim = l/v = "
                                << wtOscillation.getWzNondimensionalization()
                                << std::endl;

        *getDescriptionStream() << "Безразмерный момент инерции iz = 2I/(rho*s*l^3) = "
                                << wtOscillation.getIzNondimensional()
                                << std::endl;

        *getDescriptionStream() << "Безразмерная частота колебаний w_nond = w_avt*l/v = "
                                << wtOscillation.getWzNondimentional()
                                << std::endl;

        *getDescriptionStream() << "Статический коэффициент момента тангажа (по общей частоте колебаний) m_z__st = w0^2*I/(qsl) = "
                                << -pow(wtOscillation.getW(), 2)*wtOscillation.getMzNondimensionalization()
                                << std::endl;

        *getGnuplotGraphStream() << "Построить график mz(a):\n"
                                 << gnuplot_scripts::mz(m_wtOscillationFile, wtOscillation.getMzNondimensionalization())
                                 << std::endl;

        *m_summaryStream << m_coreName << " ";
    }

    void reportAmplitude(const WtOscillation &wtOscillation) const
    {
        *getDescriptionStream() << "#######################################################\n";

        *getDescriptionStream() << "Сохранение данных амплитуд в файл"
                                   "(время, первая  производная по времени, период, частота, индекс отсчета в исходных данных): "
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

        *getDescriptionStream() << "Максимальная амплитуда колебаний: "
                                << maxAmplitude.m_amplitudeAngle
                                << std::endl
                                << "Минимальная амплитуда колебаний: "
                                << minAmplitude.m_amplitudeAngle
                                << std::endl;

        amplitudeVector.sortViaTime();

        *getDescriptionStream() << "Сохранение данных абсолютной амплитуды в файл"
                                   "(время, угол, первая  производная по времени, период, частота, индекс отсчета в исходных данных): "
                                << std::endl;
        {
            std::ofstream fout(m_angleHistroyAbsAmplitudeFile.string());

            fout << amplitudeVector;
        }

        // ANALYSIS OF AMPLITUDE

        // todo make formal precise creteria
        const bool isAmplitudeDecreasing = maxAmplitude.m_amplitudeTime < minAmplitude.m_amplitudeTime;
        const double extrenumAmplitude = (isAmplitudeDecreasing ? minAmplitude.m_amplitudeAngle : maxAmplitude.m_amplitudeAngle);
        *getDescriptionStream() << "Характер динамики амплитуды: "
                                << (isAmplitudeDecreasing ? "падение " : "рост ")
                                << "амплитуды до " << extrenumAmplitude
                                << std::endl;

        // todo most frequent amplitude

        const double limitAmplitudeRatio = 0.1; // hardcode

        *getDescriptionStream() << "Определение предельной амплитуды с допуском (avgAmpl[1-(ampl/extrnumAmpl) < ratio]): "
                                << limitAmplitudeRatio
                                << std::endl;

        // calculate m_z__st
        // -pow(wtOscillation.getW(), 2)*wtOscillation.getMzNondimensionalization()

        AngleHistory pitchStaticCoefficient;
        *getGnuplotGraphStream() << "Построить график статического момента тангажа:\n"
                                 << gnuplot_scripts::pitchStaticCoefficient(m_angleHistroyAbsAmplitudeFile, wtOscillation.getMzNondimensionalization())
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

            *getDescriptionStream() << "Первое значение амплитуды удовлетворяющее критерию (время, угол): "
                                    << angleAmplitudeToAvg.front().m_amplitudeTime << ", "
                                    << angleAmplitudeToAvg.front().m_amplitudeAngle << "\n"
                                    << "Последнее значение амплитуды удовлетворяющее критерию (время, угол): "
                                    << angleAmplitudeToAvg.back().m_amplitudeTime << ", "
                                    << angleAmplitudeToAvg.back().m_amplitudeAngle << "\n";

            amplitude::AngleAmplitudeBase angleAmplitudeAvg = amplitude::getAvg(angleAmplitudeToAvg);

            *getDescriptionStream() << "Предельная средняя амплитуда автоколебаний [degree]: "
                                    << angleAmplitudeAvg.m_amplitudeAngle
                                    << std::endl;

            *m_summaryStream << angleAmplitudeAvg.m_amplitudeAngle << " ";

            *getDescriptionStream() << "Средний период автоколебаний T[с]: "
                                    << angleAmplitudeAvg.m_period
                                    << std::endl;

            *getDescriptionStream() << "Средняя частота автоколебаний w[рад/с]: "
                                    << angleAmplitudeAvg.m_frequency
                                    << std::endl;

            *m_summaryStream << angleAmplitudeAvg.m_frequency << " ";

            const double wzMondimentionalSummary = calcWzNondimentional(angleAmplitudeAvg.m_frequency,
                                                                 wtOscillation.getFlow(),
                                                                 wtOscillation.getModel());

            *getDescriptionStream() << "Безразмерная частота колебаний для участка автоколебаний [1]: "
                                    << wzMondimentionalSummary
                                    << std::endl;

            *m_summaryStream << wzMondimentionalSummary << " ";

            *getGnuplotGraphStream() << "Построить график амплитуды и предельной амплитуды:\n"
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

        *getDescriptionStream() << "Максимальный достигаемый угол: " << maxAngle
                                << std::endl
                                << "Минимально достигаемый угол: " << minAngle
                                << std::endl;

        *getDescriptionStream() << "Построем сечения по углам с шагом " << sectionAngleStep
                                << std::endl;

        {
            std::stringstream sectionsGnuplotFileScript;
            std::vector<boost::filesystem::path> specificSectionFileVector;
            specificSectionFileVector.reserve(sectionVector.size());

            *getDescriptionStream() << "Сохранение данных сечения в файл (время, первая, вторая, третья производые по времени): "
                                    << std::endl;

            for (const auto &section : sectionVector)
            {
                std::string specificSectionFileName = m_coreName + "_section" + std::to_string(section.getTargetAngle()) + "_" + std::string(Section::ASCENDING == section.getSectionType() ? "asc" : "desc") + ".oscillation";
                boost::filesystem::path specificSectionFile = m_outputPath / specificSectionFileName;

                specificSectionFileVector.push_back(specificSectionFile);

                *getDescriptionStream() << specificSectionFile
                                        << std::endl;

                {
                    std::ofstream fout(boost::filesystem::path(m_outputPath / specificSectionFile).string());

                    fout << section << "\n";
                }
            }

            // getMzNondimensionalization() * ddangle (w) (getWzNondimensionalization() * dangle())
            *getGnuplotGraphStream() << "Построить график m_z(a'_nondim) = (I/(qsl))*a''((l/v)*a') = :\n"
                                     << gnuplot_scripts::sections(m_wtOscillationFile,
                                                                  specificSectionFileVector,
                                                                  m_coreName,
                                                                  wtOscillation.getMzNondimensionalization(),
                                                                  wtOscillation.getWzNondimensionalization())
                                     << std::endl;
        }
    }

    void reportPeriods(const WtOscillation &wtOscillation) const
    {
        amplitude::AngleAmplitudeVector amplitudeVector = wtOscillation.getAngleAmplitudeVector();

        boost::filesystem::create_directory(m_periodsBaseDir);

        *getDescriptionStream() << "Данные периодов будут сохранены в "
                                << m_periodsBaseDir
                                << std::endl;

        std::vector<double>::const_iterator startOfPeriodIt = wtOscillation.timeBegin();

        int startIndexOfPeriodData = 0;
        int endIndexOfPeriodData = 0;
        for (int periodCounter = 0; periodCounter < amplitudeVector.size(); periodCounter++)
        {
            const double amplitudeTimeValue = amplitudeVector.getAmplitudeTime(periodCounter);
            
            std::vector<double>::const_iterator timeBorderIt =
                std::find_if(startOfPeriodIt,
                             wtOscillation.timeEnd(),
                             [&amplitudeTimeValue](const double &timeWtOsc)
                             {
                                 return timeWtOsc > amplitudeTimeValue;
                             });

            if (wtOscillation.timeEnd() == timeBorderIt)
                return;

            endIndexOfPeriodData = timeBorderIt - wtOscillation.timeBegin();

            if (endIndexOfPeriodData > wtOscillation.size())
                return;

            boost::filesystem::path singlePeriodFile = m_periodsBaseDir / std::to_string(periodCounter);

            *getDescriptionStream() << "Сохраняем данные периода колебаний на участке "
                                    << "[" << wtOscillation.getTime(startIndexOfPeriodData) << ", "
                                    << wtOscillation.getTime(endIndexOfPeriodData) << "]"
                                    << " в файл: "
                                    << singlePeriodFile
                                    << std::endl;
            std::ofstream foutSinglePeriod(singlePeriodFile.string());
            
            for (int i = startIndexOfPeriodData; i < endIndexOfPeriodData; i++)
            {           
                foutSinglePeriod << wtOscillation.getTime(i) << " "
                                 << wtOscillation.getAngle(i) << " "
                                 << wtOscillation.getDangle(i) << " "
                                 << wtOscillation.getDdangle(i) << std::endl;
            }

            startOfPeriodIt = timeBorderIt;
            startIndexOfPeriodData = endIndexOfPeriodData;
        }
        
        {
            *getGnuplotGraphStream() << "Построить график a(t) по периодам:\n";
            *getGnuplotGraphStream() << "plot "
                                     << m_wtOscillationFile << " using 1:2 with lines, ";

            for (int periodCounter = 0; periodCounter < amplitudeVector.size(); periodCounter++)
            {
                boost::filesystem::path singlePeriodFile = m_periodsBaseDir / std::to_string(periodCounter);

                *getGnuplotGraphStream() << singlePeriodFile << " using 1:2 with lines notitle, ";
            }
            *getGnuplotGraphStream() << std::endl;
        }

        {
            *getGnuplotGraphStream() << "Построить график a''(a) по периодам:\n";
            *getGnuplotGraphStream()<< "plot "
                                     << m_wtOscillationFile << " using 2:4 with lines, ";

            for (int periodCounter = 0; periodCounter < amplitudeVector.size(); periodCounter++)
            {
                boost::filesystem::path singlePeriodFile = m_periodsBaseDir / std::to_string(periodCounter);

                *getGnuplotGraphStream() << singlePeriodFile << " using 2:4 with lines notitle, ";
            }
            *getGnuplotGraphStream() << std::endl;
        }
    }

private:
    std::shared_ptr<std::stringstream> getDescriptionStream() const { return m_descriptionStreamVector.at(0); }
    std::shared_ptr<std::stringstream> getGnuplotGraphStream() const { return m_descriptionStreamVector.at(1); }

public:
    boost::filesystem::path m_wtOscillationFile;
    boost::filesystem::path m_angleHistroyAmplitudeFile;
    boost::filesystem::path m_angleHistroyAbsAmplitudeFile;
    boost::filesystem::path m_periodsBaseDir;

private:
    std::string m_coreName;
    std::string m_wtOscillationName;
    std::string m_amplitudeName;
    std::string m_absAmplitudeName;

    std::shared_ptr<std::stringstream> m_summaryStream; // todo make it shared_pt

    boost::filesystem::path m_outputPath; //< core dirrectory
};
