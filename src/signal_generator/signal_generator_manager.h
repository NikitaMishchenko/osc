#pragma once

#include <string>
#include <fstream>
#include <queue>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "signal_generator_manager.h"


/*
class SignalGeneratorManager
{
public:

    SignalGeneratorManager()
    {
        m_signalGenerator = boost::make_shared<signal_generator::SignalGenerator>();
    }

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

        std::vector<double> getParameters() const
        {
            return m_parameters;
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
                    signal_generator::DataForConstantSignal dataForConstantSignal(m_signalConfigurationQueue.front().getParameters());

                    m_signalGenerator->makeConstantSignal(dataForConstantSignal);

                    break;
                }

                case (signal_generator::Operations::MULTIPLY_HARMONIC):
                {
                    signal_generator::DataForMultiplyAddHarmonic dataForMultiplyAddHarmonic(m_signalConfigurationQueue.front().getParameters());

                    m_signalGenerator->multiplyHarmonic(dataForMultiplyAddHarmonic);

                    break;
                }

                case (signal_generator::Operations::ADD_MAKE_HARMONIC):
                {
                    signal_generator::DataForMultiplyAddHarmonic dataForMultiplyAddHarmonic(m_signalConfigurationQueue.front().getParameters());

                    m_signalGenerator->addHarmonic(dataForMultiplyAddHarmonic);

                    break;
                }

                case (signal_generator::Operations::ADD_SLOPE_LINNEAR):
                {
                    signal_generator::DataForSlopeLinnear dataForSlopeLinnear(m_signalConfigurationQueue.front().getParameters());

                    m_signalGenerator->addSlopeLinnear(dataForSlopeLinnear);

                    break;
                }

                case (signal_generator::Operations::MULTIPLY_SLOPE_LINNEAR):
                {
                    signal_generator::DataForSlopeLinnear dataForSlopeLinnear(m_signalConfigurationQueue.front().getParameters());

                    m_signalGenerator->multiplySlopeLinnear(dataForSlopeLinnear);

                    break;
                }

                case (signal_generator::Operations::SCALE_TIME):
                {
                    signal_generator::DataForScale dataForScale(m_signalConfigurationQueue.front().getParameters());

                    m_signalGenerator->scaleTime(dataForScale);

                    break;
                }

                case (signal_generator::Operations::SCALE_ANGLE):
                {
                    signal_generator::DataForScale dataForScale(m_signalConfigurationQueue.front().getParameters());

                    m_signalGenerator->scaleAngle(dataForScale);

                    break;
                }

                case (signal_generator::Operations::END):
                {
                    m_signalConfigurationQueue.front();

                    std::cout << "Operation queue has been proceeded\n";

                    m_signalGenerator->save("signalGeneratorResult");

                    return true;
                }

                case (signal_generator::Operations::UNHANDLED):
                default:
                {
                    std::cerr << "Unhandled operation in queue from file singal configuration skipping...\n";
                    break;
                }
                } // switch


                // todo SAVE make string to outputFileName


                m_signalConfigurationQueue.pop();
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
    boost::shared_ptr<signal_generator::SignalGenerator> m_signalGenerator;
};
*/