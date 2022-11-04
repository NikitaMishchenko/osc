#pragma once

#include <istream>
#include <queue>
#include <memory>

#include "operation_type_parsers.h"
#include "signal_generator_base.h"
#include "operations_factory.h"

class OperationsQueue
{
public:
    OperationsQueue() : m_signalGenerator(signal_generator::SignalGenerator())
    {
    }

    bool loadOperationsConfiguration(const std::string &fileName)
    {
        std::ifstream fin(fileName);

        if (!fin.is_open())
            throw std::string("OperationQueue cant load file:" + fileName);

        OperationsFactory factory(fileName);

        std::shared_ptr<OperationPerformer> operationPerformerPtr;
        int factoryErrCode;

        do
        {
            std::tie(factoryErrCode, operationPerformerPtr) = factory.createOperation();

            std::cout << "factory createOperation errCode: " << factoryErrCode << "\n";

            m_operations.push(operationPerformerPtr);

        } while (FactoryCode::WORK_SUCCEED == factoryErrCode);

        std::cout << "OperationsQueue loaded with size of " << m_operations.size() << "\n";

        fin.close();

        return true;
    }

    void performOperations()
    {
        std::cout << "performing queue Operations\n";

        signal_generator::SignalGenerator *signalGeneratorPtr;

        while (!m_operations.empty() || m_operations.front().getOperationType() != Operations::END)
        {
            signalGeneratorPtr = m_operations.front().perform(signalGeneratorPtr);

            m_operations.pop();
        }
    }

private:
    std::queue<std::shared_ptr<OperationPerformer> > m_operations;
    signal_generator::SignalGenerator m_signalGenerator;
};
