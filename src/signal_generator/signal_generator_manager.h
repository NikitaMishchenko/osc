#pragma once

#include <string>
#include <fstream>
#include <queue>

#include "signal_generator_manager.h"

namespace
{
    int operationFromString(const std::string &operationString)
    {
        int operationEnum;

        if ("constantSignal:" == operationString)
            return signal_generator::Operations::MAKE_CONSTANT_SIGNAL;

        else if ("multiplyHarmonic:" == operationString)
            return signal_generator::Operations::MULTIPLY_HARMONIC;

        else if ("addHarmonic:" == operationString)
            return signal_generator::Operations::ADD_MAKE_HARMONIC;

        else if ("slopeLinnear:" == operationString)
            return signal_generator::Operations::SLOPE_LINNERAR;

        else if ("scaleTime:" == operationString)
            return signal_generator::Operations::SCALE_TIME;

        else if ("scaleAngle:" == operationString)
            return signal_generator::Operations::SCALE_ANGLE;

        else if ("End." == operationString)
            return signal_generator::Operations::END;

        else
            return signal_generator::Operations::UNHANDLED;
    };

    std::vector<double> argumentsFromStream(std::ifstream &source, const int operationToPerform)
    {
        switch (operationToPerform)
        {
            case (signal_generator::Operations::MAKE_CONSTANT_SIGNAL):
            {
                std::vector<double> dataForConstantSignal;

                double d_buff;
                std::string s_buff;

                for (int i = 0; i < 3; ++i) // hardcode
                {   
                    source >> s_buff >> d_buff;
                    std::cout << s_buff << " " << d_buff << "\n";

                    dataForConstantSignal.push_back(d_buff);
                }

                return dataForConstantSignal;
            }

            case (signal_generator::Operations::MULTIPLY_HARMONIC):
            case (signal_generator::Operations::ADD_MAKE_HARMONIC):
            {
                std::vector<double> dataForMultiplyAddHarmonic;

                double d_buff;
                std::string s_buff;

                for (int i = 0; i < 5; ++i) // hardcode
                {   
                    source >> s_buff >> d_buff;
                    std::cout << s_buff << " " << d_buff << "\n";

                    dataForMultiplyAddHarmonic.push_back(d_buff);
                }

                return dataForMultiplyAddHarmonic;
            }

            case (signal_generator::Operations::SLOPE_LINNERAR):
            {

                std::vector<double> dataForSlopeLinnear;

                double d_buff;
                std::string s_buff;

                for (int i = 0; i < 4; ++i) // hardcode
                {   
                    source >> s_buff >> d_buff;
                    std::cout << s_buff << " " << d_buff << "\n";

                    dataForSlopeLinnear.push_back(d_buff);
                }

                return dataForSlopeLinnear;
            }

            case (signal_generator::Operations::SCALE_TIME):
            case (signal_generator::Operations::SCALE_ANGLE):
            {
                std::vector<double> dataForScale;

                double d_buff;
                std::string s_buff;

                for (int i = 0; i < 3; ++i) // hardcode
                {   
                    source >> s_buff >> d_buff;
                    std::cout << s_buff << " " << d_buff << "\n";
                    
                    dataForScale.push_back(d_buff);
                }

                return dataForScale;
            }

            case (signal_generator::Operations::END):
            {

                break;
            }
        }

        return std::vector<double>();
    }
}

class SignalGeneratorManager
{
public:
    class SignalConfiguration
    {
    public:
        SignalConfiguration() : m_isLast(true)
        {
        }

        int load(std::ifstream &source)
        {
            std::string stringBuff;

            source >> stringBuff;

            std::cout << "stringBuff: " << stringBuff << "\n";

            m_operation = operationFromString(stringBuff);
            
            std::cout << "operationFound: " << m_operation << "\n";
            
            m_parameters = argumentsFromStream(source, m_operation);

            return m_operation;
        }

        int getOperation() const
        {
            return m_operation;
        }

        // DataForConstantSignal getDataForConstantSignal()

    private:
        bool m_isLast;
        int m_operation;
        std::vector<double> m_parameters;
    };

    bool doWork()
    {
        if (!m_signalConfigurationQueue.empty())
        {
            while (!m_signalConfigurationQueue.empty())
            {
                int operationToPerform = m_signalConfigurationQueue.front().getOperation();

                switch (operationToPerform)
                {
                case (signal_generator::Operations::MAKE_CONSTANT_SIGNAL):
                {
                    signal_generator::DataForConstantSignal dataForConstantSignal;

                    m_signalConfigurationQueue.front().getOperation();

                    signalGenerator->makeConstantSignal(dataForConstantSignal); // m_signalConfigurationQueue.front();
                    break;
                }

                case (signal_generator::Operations::MULTIPLY_HARMONIC):
                {
                    m_signalConfigurationQueue.front();
                    break;
                }

                case (signal_generator::Operations::ADD_MAKE_HARMONIC):
                {
                    m_signalConfigurationQueue.front();
                    break;
                }

                case (signal_generator::Operations::SLOPE_LINNERAR):
                {
                    m_signalConfigurationQueue.front();
                    break;
                }

                case (signal_generator::Operations::SCALE_TIME):
                {
                    m_signalConfigurationQueue.front();
                    break;
                }

                case (signal_generator::Operations::SCALE_ANGLE):
                {
                    m_signalConfigurationQueue.front();
                    break;
                }

                case (signal_generator::Operations::END):
                {
                    m_signalConfigurationQueue.front();
                    break;
                }

                case (signal_generator::Operations::UNHANDLED):
                {
                    m_signalConfigurationQueue.front();
                    break;
                }

                default:

                    m_signalConfigurationQueue.pop();
                }
            }

            return true;
        }

        return false;
    }

    bool loadSignalConfiguaration(const std::string &fileName)
    {
        std::ifstream fin(fileName);

        std::cout << fileName;
        
        if (fin.is_open()) 
            std::cout << "opend succesfully\n"; 
        else
            std::cout << "not opend\n";

        SignalConfiguration operation;

        do
        {
            operation.load(fin);

            if (signal_generator::Operations::UNHANDLED != operation.getOperation())
                m_signalConfigurationQueue.push(operation);
            else
                std::cout << "unhandled operation appeared... will skip it\n";

        } while (signal_generator::Operations::END != operation.getOperation());

        fin.close();

        return true;
    }

private:
    std::queue<SignalConfiguration> m_signalConfigurationQueue;
    boost::shared_ptr<signal_generator::SignalGenerator> signalGenerator;
};
