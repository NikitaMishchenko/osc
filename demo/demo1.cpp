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

void doJob(std::string coreName)
{   
    boost::filesystem::path root = "/home/mishnic/data/phd/sphere_cone_M1.75/"  + coreName;
    std::stringstream descriptionStream;
    
    boost::filesystem::path fileToProceed = root.string();
    boost::filesystem::path workingPath = root;

    ///
    //***********************************************************************************************
    ///

    descriptionStream << "Определяем корневое имя файла: "
                      << "\"" << coreName<< "\""
                      << std::endl;

    ///
    //***********************************************************************************************
    ///

    std::string specificInitialFile = coreName + ".angle_history";
    std::string descriptionFileName = coreName + ".discription";
    
    descriptionStream << "Определяем имя файла истории колебаний: "
                      << "\"" << specificInitialFile<< "\""
                      << std::endl;
    
    descriptionStream << "Определяем имя файла описания процедуры обработки: "
                      << "\"" << descriptionFileName<< "\""
                      << std::endl;

    
    ///
    //***********************************************************************************************
    ///

    descriptionStream << "Предварительный рассчет параметров колебаний для файла: "
                      << "\"" << specificInitialFile <<  "\""
                      << std::endl;

    Oscillation oscillation(AngleHistory(fileToProceed.string() + "/" + specificInitialFile));

    ///
    //***********************************************************************************************
    ///

    std::string modelFileName = "shpereCone1.model";

    descriptionStream << "Загрузка параметров модели из файла: "
                      << "\"" << modelFileName << "\""
                      << std::endl;

    Model model;
    model.loadFile(root.string() + "/" + modelFileName);

    descriptionStream << "Параметры модели:\n" << model.getInfoString() << std::endl;

    ///
    //***********************************************************************************************
    ///

    std::string flowFileName = coreName + ".flow";

    descriptionStream << "Загрузка параметров потока из файла: "
                      << "\"" << flowFileName << "\""
                      << std::endl;

    wt_flow::Flow flow(workingPath.string() + "/" + flowFileName);

    descriptionStream << "Параметры потока:\n" << flow.getInfoString() << std::endl;

    ///
    //***********************************************************************************************
    ///

    WtOscillation wtOscillation(oscillation, flow, model);

    std::string specificWtOscFile = coreName +  ".wt_oscillation";

    descriptionStream << "Сохранение данных колебаний в файл: "
                      << "\"" << specificWtOscFile << "\""
                      << std::endl;

    
    {
        std::ofstream fout(workingPath.string() + "/" + specificWtOscFile);

        fout << wtOscillation << "\n";
    }

    descriptionStream << "Коэффициент обезразмеривания для получения mz = I/(qsl)a'' -> I/(qsl) = " 
                      << wtOscillation.getMzNondimensionalization()
                      << std::endl;
    
    descriptionStream << "Безразмерный момент инерции iz = 2I/(rho*s*l) = " 
                      << wtOscillation.getIzNondimensional()
                      << std::endl;

    descriptionStream << "Безразмерная частота колебаний w = w_avt*l/v = " 
                      << wtOscillation.getW()*wtOscillation.getModel().getL()/wtOscillation.getFlow().getVelocity()
                      << std::endl;


    ///
    //***********************************************************************************************
    ///
/*
    double sectionAngle = 0; // 0 - 20

    descriptionStream << "Рассчет методом сечений для угла " << sectionAngle << " градусов\n"; 

    std::string specificSectionFile = coreName + "_section" + std::to_string(sectionAngle) + ".oscillation";

    bool isOk = false;
    Oscillation section;

    std::tie(isOk, section) = makeSection(oscillation, sectionAngle);
    
    descriptionStream << "Сохранение данных секции в файл: "
                      << "\"" << specificSectionFile << "\""
                      << std::endl;
    
    {
        std::ofstream fout(workingPath.string() + "/" + specificSectionFile);

        fout << section << "\n";
    }
*/
    descriptionStream << "Построить график a''(a'):\n"
                      << "plot \"" << specificWtOscFile << "\" using 4:3 with linespoints, \"" // << specificSectionFile << "\" using 4:3"
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

    {
        std::ofstream fout(workingPath.string() + "/" + specificAmplitudeFile);

        AngleHistory amplitude(wtOscillation.getTimeAmplitude(), wtOscillation.getAngleAmplitude());

        fout << amplitude << "\n";
    }

    ///
    //***********************************************************************************************
    ///

    descriptionStream << "Рассчет амплитуды колебаний"
                      << std::endl;

    amplitude::AngleAmplitudeAnalyser angleAmplitudeAnalyser(amplitude::AngleAmplitude(std::make_shared<std::vector<double>>(wtOscillation.getTime()), 
                                                                                       std::make_shared<std::vector<double>>(wtOscillation.getAngle()), 
                                                                                       std::make_shared<std::vector<double>>(wtOscillation.getDangle())));
       
    std::vector<amplitude::AngleAmplitudeBase> sortedAmplitude = angleAmplitudeAnalyser.getSortedAmplitude();
  

    {
        

        /*AngleHistory amplitude(wtOscillation.getTimeAmplitude(), wtOscillation.getAngleAmplitude());

        struct AbsAmplitude
        {
            AbsAmplitude(double _time, double _angle) : time(_time), angle(std::abs(_angle))
            {}

        bool operator < (const AbsAmplitude& rhs) const
        {
            return time < rhs.time;
        }

            double time;
            double angle;
        };

        std::vector<AbsAmplitude> aHvector;

        for (int i = 0; i < amplitude.size(); i++)
        {
            aHvector.push_back(AbsAmplitude(amplitude.getTime(i), amplitude.getAngle(i)));
        }

        std::sort(aHvector.begin(), aHvector.end());

        std::ofstream fout(workingPath.string() + "/" + specificAbsAmplitudeFile);

        for (auto aa : aHvector)
            fout << aa.time << "\t" << aa.angle << "\n";
        */
        
        std::string specificAbsAmplitudeFile = coreName + "_abs.amplitude";
        std::ofstream fout(workingPath.string() + "/" + specificAbsAmplitudeFile);
        
        descriptionStream << "Сохранение данных абсолютной амплитуды в файл: "
                            << "\"" << specificAbsAmplitudeFile << "\""
                            << std::endl;

       for (auto sAmp : sortedAmplitude)
            fout << sAmp.m_amplitudeTime << "\t" << sAmp.m_amplitudeAngle << "\n";
    }


    ///
    //***********************************************************************************************
    ///

    {
        std::cout << "Сохранение информации по обработке данных в файл: "
                  << "\"" << workingPath.string() + "/" + descriptionFileName << "\"" 
                  << std::endl;

        std::ofstream fout(workingPath.string() + "/" + descriptionFileName);

        fout << descriptionStream.str() << "\n";
    }


    // /home/mishnic/data/phd/data_proc/pic_ddangle_respect_to_angle_of_attack
    
    ///
    //***********************************************************************************************
    ///

    
}

int main(int argc, char** argv)
{
    std::vector<int> dataIndex = {4463, 4470, 4474, 4465, 4468, 4471, 4472};
    
    for (auto index : dataIndex)
        doJob(std::to_string(index));
    
}