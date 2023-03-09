#include <iostream>

#include <string>
#include <tuple>

#include "utils/function_generator/function_probe_data.h"
#include "core/function.h"

namespace function_generator
{

    enum Mode
    {
        DEFAULT,
        SAVE_FILE
    };

    struct InputData
    {
        double m_time0Vector;
        size_t m_sizeVector;
        double m_dtVector;
        std::vector<double> m_coeffVector;
    };

    class FunctionGenerator
    {
    public:
        FunctionGenerator(const std::string basicFileName = "test") : m_basicFileName(basicFileName), m_mode(SAVE_FILE)
        {
        }

        void setMode(const int mode)
        {
            m_mode = mode;
        }

        void setBasicFileName(const std::string &basicFileName)
        {
            m_basicFileName = basicFileName;
        }

        std::tuple<bool, std::vector<Function>>
        actOnData(std::vector<FunctionProbeData> &functionProbeDataVector)
        {
            std::string plotterScript = "plot ";

            const int numberOfTests = functionProbeDataVector.size();

            std::vector<Function> functionVector;

            if (numberOfTests < 1)
            {
                std::cerr << "WARNING SIZE PROBLEM! " << numberOfTests << "\n";
                return std::make_tuple(false, functionVector);
            }

            for (size_t i = 0; i < numberOfTests; i++)
            {
                std::string fileName = m_basicFileName + std::to_string(i);

                functionProbeDataVector.at(i).setFileName(fileName);

                functionVector.push_back(actAndSaveData(functionProbeDataVector.at(i)));

                plotterScript += appendPlotterScript(fileName);

                if (numberOfTests - 1 != i)
                    plotterScript += ", ";
            }

            std::string scriptFileName = "plotScript";

            std::ofstream fout(scriptFileName + "_" + m_basicFileName);

            fout << plotterScript;

            std::cout << plotterScript << "\n";

            return std::make_tuple(true, functionVector);
        }

    private:
        std::tuple<std::vector<double>, std::vector<double>>
        getFuncionResult(double (*function)(double, std::vector<double>),
                         const FunctionProbeData &functionProbeData)
        {
            std::vector<double> func;
            std::vector<double> time;

            for (size_t i = 0; i < functionProbeData.m_length; i++)
            {
                double t = i * functionProbeData.m_dt;

                time.push_back(functionProbeData.m_time0 + t);
                func.push_back(function(t, functionProbeData.m_coeff));
            }

            return std::make_tuple(time, func);
        }

        // arg, unction
        Function actAndSaveData(const FunctionProbeData &functionProbeData)
        {
            std::vector<double> func;
            std::vector<double> arg;

            std::tie(arg, func) = getFuncionResult(functionProbeData.m_function,
                                                   functionProbeData);

            std::cout << "trying to save data to file " << functionProbeData.m_fileName << "\n";

            if (SAVE_FILE == m_mode)
            {
                std::ofstream fout(functionProbeData.m_fileName);

                for (size_t i = 0; i < func.size(); i++)
                {
                    fout << arg.at(i) << "\t"
                         << func.at(i) << "\n";
                }
            }

            Function function(arg, func);

            return function;
        }

        std::string appendPlotterScript(const std::string &fileName)
        {
            std::string result = "\"" + fileName + "\" " + "using 1:2 with linespoints";
            return result;
        }

        std::string m_basicFileName;
        int m_mode;
    };

} // namespace function_generator
