#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "oscillation/wt_oscillation.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"

class ProcessorOutput
{
public:
    ProcessorOutput(const boost::filesystem::path &outputPath,
                    const std::string &coreName) : m_coreName(coreName),
                                                   m_outputPath(outputPath)
    {
        // working in folder of specific core data
        m_outputPath /= coreName;

        m_wtOscillationName = coreName + ".wt_oscillation";
        m_amplitudeName = coreName + "_amplitude.angle_history";

        m_wtOscillationFile = outputPath / m_wtOscillationName;
    }

    std::tuple<bool, std::string> write(const WtOscillation &wtOscillation)
    {

        m_descriptionStream << "Сохранение данных колебаний в файл: "
                            << "\"" << m_wtOscillationFile << "\""
                            << std::endl;

        {
            std::ofstream fout(m_wtOscillationFile.string());

            fout << wtOscillation << "\n";
        }

        m_descriptionStream << "Частота колебаний w[Гц] = "
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

        return std::make_tuple(true, m_descriptionStream.str());
    }

protected:
private:
    std::string m_coreName;
    std::string m_wtOscillationName;
    std::string m_amplitudeName;

    boost::filesystem::path m_wtOscillationFile;

    boost::filesystem::path m_outputPath;
    mutable std::stringstream m_descriptionStream;
};