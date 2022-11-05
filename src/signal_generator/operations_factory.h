#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <tuple>
#include <memory>

//#include "operation_types.h"

#include "operation_performer.h"
#include "operation_parser.h"

enum FactoryCode
{
    WORK_DONE,
    WORK_SUCCEED,
    WORK_FAILED
};

class OperationsFactory
{
public:
    OperationsFactory()
    {
    }

    OperationsFactory(const std::string &fileName)
    {
        openFileSource(fileName); // err check
    }

    ~OperationsFactory()
    {
        m_source.close();
    }

    std::tuple<int, std::shared_ptr<OperationPerformer>> createOperation()
    {
        OperationParser operationParser;

        int errCodeParser;
        OperationContainer operationContainer;

        std::tie(errCodeParser, operationContainer) = operationParser.getNextOperationData(m_source);

        std::cout << "Parsed single operation errCode: " << errCodeParser << "\n";

        int errCodeFactory = FactoryCode::WORK_SUCCEED; // todo refactor

        if (parser::isBadCode(errCodeParser))
            errCodeFactory = FactoryCode::WORK_FAILED;

        if (ParserCode::NEGATIVE == errCodeParser || ParserCode::END_OPERATION_ACQUIRED == errCodeParser)
            errCodeFactory = FactoryCode::WORK_DONE;

        // try catch
        std::shared_ptr<OperationPerformer> operationPerformerPtr = getOperationPerformer(operationContainer.data,
                                                                                          operationContainer.type);

        std::cout << "created operation of type " << operationContainer.type << ", errCode: " << errCodeFactory << "\n";

        return std::make_tuple(errCodeFactory, operationPerformerPtr);
    }

private:
    bool openFileSource(const std::string &fileName)
    {
        m_source.open(fileName);

        if (!m_source.is_open())
            return false;

        return true;
    }

    std::fstream m_source;
};
