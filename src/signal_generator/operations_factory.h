#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <tuple>
#include <memory>

//#include "operation_types.h"

#include "operation_performer.h"
#include "operation_parser.h"

namespace
{
    std::shared_ptr<OperationPerformer>
    getOperationPerformer(const std::vector<double> &operationData,
                          Operations type)
    {
        std::cout << "getOperationPerformer()";

        switch (type)
        {
        case (Operations::MAKE_CONSTANT_SIGNAL):
            return std::make_shared<MakeConstantSignal>(operationData);

        case (Operations::SCALE_TIME):
            return std::make_shared<ScaleTime>(operationData);

        case (Operations::SCALE_ANGLE):
            return std::make_shared<ScaleAngle>(operationData);

        case (Operations::MULTIPLY_HARMONIC):
            return std::make_shared<MultiplyHarmonic>(operationData);

        case (Operations::ADD_MAKE_HARMONIC):
            return std::make_shared<AddHarmonic>(operationData);

        case (Operations::ADD_SLOPE_LINNEAR):
            return std::make_shared<AddSlopeLinnear>(operationData);

        case (Operations::MULTIPLY_SLOPE_LINNEAR):
            return std::make_shared<MultiplySlopeLinnear>(operationData);

        case (Operations::END):
            return std::make_shared<OperationPerformer>();

        default:
            std::cerr << "smth wrong!";
        }

        return std::make_shared<OperationPerformer>();
    }
}

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
