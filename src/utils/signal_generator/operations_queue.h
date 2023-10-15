#pragma once

#include <istream>
#include <queue>
#include <memory>

#include <boost/filesystem.hpp>

#include "signal_generator_base.h"
#include "operations_factory.h"

/**
 * Aplying of operatoions over signalGenerator data
 * accorfing to config file (see OperationsFactory)
 */
class OperationsQueue
{
public:
    OperationsQueue() : m_signalGenerator(signal_generator::SignalGenerator())
    {
    }

    bool loadOperationsConfiguration(const std::string &fileName)
    {
        if (!boost::filesystem::exists(fileName))
            throw std::string("OperationQueue cant load file:" + fileName);

        OperationsFactory factory(fileName);

        std::shared_ptr<OperationPerformer> operationPerformerPtr;
        int factoryErrCode;

        do
        {
            std::tie(factoryErrCode, operationPerformerPtr) = factory.createOperation();

            std::cout << "factory createOperation errCode: " << factoryErrCode << "\n";

            m_operations.push(operationPerformerPtr);

        } while (FactoryCode::WORK_DONE != factoryErrCode);

        std::cout << "OperationsQueue loaded with size of " << m_operations.size() << "\n";

        return true;
    }

    void performOperations()
    {
        std::cout << "performing queue Operations\n";

        signal_generator::SignalGenerator *signalGeneratorPtr = &m_signalGenerator; // todo use smart ptr

        while (!m_operations.empty())
        {
            if (m_operations.front()->getOperationType() == Operations::END)
                break;

            signalGeneratorPtr = m_operations.front()->perform(signalGeneratorPtr);

            signalGeneratorPtr->info();

            m_operations.pop();
        }
    }

    AngleHistory getAngleHistory() const
    {
        return m_signalGenerator.getAngleHistory();
    }

private:
    std::queue<std::shared_ptr<OperationPerformer>> m_operations;
    signal_generator::SignalGenerator m_signalGenerator;
};
