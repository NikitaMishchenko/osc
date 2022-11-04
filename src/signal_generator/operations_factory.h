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
    getOperationPerformer(const std::vector<double>& operationData,
                                             Operations type)
    {

        switch (type)
        {
        case (Operations::MAKE_CONSTANT_SIGNAL):
            return std::make_shared<MakeConstantSignal>(operationData, type);
        
        /*case (Operations::SCALE_TIME):
            return ScaleTime(operationData);

        case (Operations::SCALE_ANGLE):
            return ScaleAngle(operationData);

        case (Operations::MULTIPLY_HARMONIC):
            return MultiplyHarmonic(operationData);

        case (Operations::ADD_MAKE_HARMONIC):
            return AddHarmonic(operationData);

        case (Operations::ADD_SLOPE_LINNEAR):
            return AddSlopeLinnear(operationData);

        case (Operations::MULTIPLY_SLOPE_LINNEAR):
            return MultiplyHarmonic(operationData);

        case (Operations::END):
            throw "END Operation can't perform";

        default:
            std::cerr << "smth wrong!";
        }

        return (new OperationPerformer());*/
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

    std::tuple<int, std::shared_ptr<OperationPerformer> > createOperation()
    {
        OperationParser operationParser;

        int errCodeParser;
        OperationContainer operationContainer;
        
        std::tie(errCodeParser, operationContainer) = operationParser.getNextOperationData(m_source); 
 
        std::cout << "Parsed single operation errCode: " << errCodeParser << "\n";

        int errCodeFactory;

        if ( parser::isBadCode(errCodeParser))
            errCodeFactory = FactoryCode::WORK_FAILED;
        
        if (errCodeParser == ParserCode::NEGATIVE) // it was last data from source
            errCodeFactory = FactoryCode::WORK_DONE;

        // создать операцию по конректному типу
        std::shared_ptr<OperationPerformer> operationPerformerPtr = getOperationPerformer(operationContainer.data,
                                                                       operationContainer.type);

        return std::make_tuple(FactoryCode::WORK_SUCCEED, operationPerformerPtr);
    }


private:
    bool openFileSource(const std::string& fileName)
    {
        m_source.open(fileName);

        if (!m_source.is_open())
            return false;

        return true;            
    }

    std::fstream m_source;
};
