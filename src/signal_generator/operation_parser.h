#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include "operation_types.h"

namespace
{
    Operations operationFromString(const std::string &operationString)
    {
        std::cout << "operationFromString: " << operationString << "\n";

        if ("constantSignal:" == operationString)
            return Operations::MAKE_CONSTANT_SIGNAL;

        else if ("multiplyHarmonic:" == operationString)
            return Operations::MULTIPLY_HARMONIC;

        else if ("addHarmonic:" == operationString)
            return Operations::ADD_MAKE_HARMONIC;

        else if ("multiplySlopeLinnear:" == operationString)
            return Operations::MULTIPLY_SLOPE_LINNEAR;

        else if ("addSlopeLinnear:" == operationString)
            return Operations::ADD_SLOPE_LINNEAR;

        else if ("scaleTime:" == operationString)
            return Operations::SCALE_TIME;

        else if ("scaleAngle:" == operationString)
            return Operations::SCALE_ANGLE;

        else if ("End." == operationString)
            return Operations::END;

        else
            return Operations::UNHANDLED;
    };

    int getOperationDataPortionCount(Operations type)
    {
        switch (type)
        {
        case (Operations::MAKE_CONSTANT_SIGNAL):
        case (Operations::SCALE_TIME):
        case (Operations::SCALE_ANGLE):
            return 3;

        case (Operations::ADD_SLOPE_LINNEAR):
        case (Operations::MULTIPLY_SLOPE_LINNEAR):
            return 4;

        case (Operations::MULTIPLY_HARMONIC):
        case (Operations::ADD_MAKE_HARMONIC):
            return 5;

        case (Operations::END):
        default:
            return 0;
        }
        return 0;
    }

} // namespace

enum ParserCode
{
    NEGATIVE,
    POSITIVE,
    END_OPERATION_ACQUIRED,
    
    // bad codes
    NO_DATA_IN_SOURCE,
    NOT_ENOUGH_DATA_IN_SOURCE
};

namespace parser
{
    inline bool isBadCode(int errCode)
    {
        return (NEGATIVE != errCode
        && POSITIVE != errCode
        && END_OPERATION_ACQUIRED != errCode);
    }
} // namespace parser

class OperationParser // base
{
public:
    OperationParser() {}
    ~OperationParser() {}

    std::tuple<int, OperationContainer> getNextOperationData(std::fstream &source)
    {
        std::cout << "getNextOperationData() ";

        int errCode = parseSingle(source);

        OperationContainer operationContainer;

        operationContainer.type = m_operationType;
        operationContainer.data = m_operationData;

        if ( Operations::END == m_operationType)
            errCode = ParserCode::END_OPERATION_ACQUIRED;

        std::cout << "errCode: " << errCode << "\n";


        return std::make_tuple(errCode, operationContainer); // report err and source info
    }

private:
    // NEGATIVE = endOfFile, POSITIVE is any data
    int isSourceEmpty(std::fstream &source) const
    {
        return (!source.eof() ? ParserCode::POSITIVE : ParserCode::NEGATIVE);
    }

    int parseSingle(std::fstream &source)
    {
        std::cout << "parseSingle()\n";

        int errCode = parseOperationType(source);

        std::cout << "\ttype: " << m_operationType << "\n";

        if (ParserCode::POSITIVE != errCode)
            return errCode;

        errCode = parseOperationData(source);

        if (ParserCode::POSITIVE != errCode)
            return errCode;

        if (ParserCode::POSITIVE == errCode)
            errCode = isSourceEmpty(source); // report is any data in source

        return errCode;
    }

    // valid source
    int parseOperationType(std::fstream &source)
    {
        std::string buff_s;

        if (source.eof())
            return ParserCode::NOT_ENOUGH_DATA_IN_SOURCE;

        source >> buff_s;

        m_operationType = operationFromString(buff_s);

        return ParserCode::POSITIVE;
    }

    int parseOperationData(std::fstream &source)
    {
        std::cout << "parseOperationData()\n";

        m_operationDataPortionsToGet = getOperationDataPortionCount(m_operationType);

        std::cout << "\tm_operationDataPortionsToGet: " << m_operationDataPortionsToGet << "\n";

        m_operationData.reserve(m_operationDataPortionsToGet);

        std::string s_buff;
        double d_buff;

        for (int i = 0; i < m_operationDataPortionsToGet; ++i)
        {
            if (source.eof())
                return ParserCode::NOT_ENOUGH_DATA_IN_SOURCE;

            source >> s_buff;
            std::cout << "\t got: " << s_buff << ", ";

            if (source.eof())
                return ParserCode::NOT_ENOUGH_DATA_IN_SOURCE;

            source >> d_buff;

            std::cout << "d_buff: " << d_buff << "\n";

            m_operationData.push_back(d_buff);
        }

        return ParserCode::POSITIVE;
    }

    Operations m_operationType;          //  тип операции в соответсвии с энумом
    int m_operationDataPortionsToGet;    // количество данных для загрузки для представления текущей операции
    std::vector<double> m_operationData; // данные для предоставления операции

    // todo data splitter
};
