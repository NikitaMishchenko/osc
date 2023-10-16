#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"

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

int main(int argc, char** argv)
{
    boost::filesystem::path root = "/home/mishnic/data/phd/sphere_cone_M1.75";
    std::stringstream descriptionStream;
    boost::filesystem::path fileToProceed = root.string();
    boost::filesystem::path workingPath = root;

    ///
    //***********************************************************************************************
    ///

    std::string coreName = "4463";

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


    ///
    //***********************************************************************************************
    ///

    double sectionAngle = 30; // 0 - 20

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

    descriptionStream << "Построить график a''(a'):\n"
                      << "plot \"" << specificWtOscFile << "\" using 4:3 with linespoints, \"" << specificSectionFile << "\" using 4:3"
                      << std::endl;


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

    ///
    //***********************************************************************************************
    ///

    
}