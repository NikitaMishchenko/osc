#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"


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

    wt_flow::Flow flow(flowFileName);

    descriptionStream << "Параметры потока:\n" << flow.getInfoString() << std::endl;

    ///
    //***********************************************************************************************
    ///

    WtOscillation wtOscillation(oscillation, flow, model);

    std::string specificWtOscFile = "4463.wt_oscillation";

    descriptionStream << "Сохранение данных колеабний в файл: "
                      << "\"" << specificWtOscFile << "\""
                      << std::endl;

    
    {
        std::ofstream fout(workingPath.string() + "/" + specificWtOscFile);

        fout << wtOscillation << "\n";
    }

    ///
    //***********************************************************************************************
    ///




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