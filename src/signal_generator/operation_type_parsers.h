#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "signal_generator_base.h"
#include "operation_types.h"
#include "operation_performer.h"

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

    std::vector<double> argumentsFromStream(std::ifstream &source, const int operationToPerform, size_t argumentsCount)
    {
        switch (operationToPerform)
        {
        case (Operations::MAKE_CONSTANT_SIGNAL):
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

        case (Operations::MULTIPLY_HARMONIC):
        case (Operations::ADD_MAKE_HARMONIC):
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

        case (Operations::ADD_SLOPE_LINNEAR):
        case (Operations::MULTIPLY_SLOPE_LINNEAR):
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

        case (Operations::SCALE_TIME):
        case (Operations::SCALE_ANGLE):
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

        case (Operations::END):
        {
            break;
        }
        }

        return std::vector<double>();
    }

    size_t argumentsCountForOperation(const int operation)
    {
        switch (operation)
        {
        case (Operations::MAKE_CONSTANT_SIGNAL):
        case (Operations::SCALE_TIME):
        case (Operations::SCALE_ANGLE):
            return 3;

        case (Operations::MULTIPLY_HARMONIC):
        case (Operations::ADD_MAKE_HARMONIC):
            return 5;

        case (Operations::ADD_SLOPE_LINNEAR):
        case (Operations::MULTIPLY_SLOPE_LINNEAR):
            return 4;

        case (Operations::END):
        default:
            return 0;
        }
        return 0;
    }

}

class OperationType
{
public:
    OperationType()
    {
        m_operationRepresentation = std::string();
        m_specOperation = Operations::UNHANDLED;
    }

    bool loadFromSource(std::istream &source)
    {
        if (source.eof())
            return false;

        bool isOk = false;

        isOk = operationFromSource(source);

        isOk = argumentsFromSource(source);

        if (!isOk)
            std::cerr << "dailed to load operation trying to load other!";

        return true;
    }

    Operations getOperationType()
    {
        return m_specOperation;
    }

    OperationPerformer getOperationPerformer()
    {
        switch (m_specOperation)
        {
        case (Operations::MAKE_CONSTANT_SIGNAL):
            return MakeConstantSignal(m_operationData);
        
        case (Operations::SCALE_TIME):
            return ScaleTime(m_operationData);

        case (Operations::SCALE_ANGLE):
            return ScaleAngle(m_operationData);

        case (Operations::MULTIPLY_HARMONIC):
            return MultiplyHarmonic(m_operationData);

        case (Operations::ADD_MAKE_HARMONIC):
            return AddHarmonic(m_operationData);

        case (Operations::ADD_SLOPE_LINNEAR):
            return AddSlopeLinnear(m_operationData);

        case (Operations::MULTIPLY_SLOPE_LINNEAR):
            return MultiplyHarmonic(m_operationData);

        case (Operations::END):
            throw "END Operation can't perform";

        default:
            std::cerr << "smth wrong!";
        }

        return OperationPerformer();
    }

    void clear()
    {
        m_specOperation = UNHANDLED;
        m_operationRepresentation = std::string();
        m_operationArgumentsCount = 0;
        m_operationData.clear();
    }

private:
    bool operationFromSource(std::istream &source)
    {
        source >> m_operationRepresentation;

        std::cout << "parser found operationRepresentation: " << m_operationRepresentation << "\n";

        m_specOperation = operationFromString(m_operationRepresentation);

        std::cout << "operation specified as: " << m_specOperation << "\n";

        if (m_specOperation == UNHANDLED)
            return false;

        m_operationArgumentsCount = argumentsCountForOperation(m_specOperation);

        m_operationData.reserve(m_operationArgumentsCount);

        return true;
    }

    bool argumentsFromSource(std::istream &source)
    {
        std::cout << "argumentsFromString():\n";

        size_t cnt = m_operationArgumentsCount;

        while (!source.eof() && cnt != 0)
        {
            double d_buff;
            std::string s_buff;

            source >> s_buff >> d_buff;
            std::cout << "\tcnt: " <<  cnt << " " << s_buff << " " << d_buff << "\n";

            m_operationData.push_back(d_buff);

            --cnt;
        }

        if (cnt)
            return false;

        std::cout << "operations parsed successfully\n";

        return true;
    }

    Operations m_specOperation;
    std::string m_operationRepresentation;
    size_t m_operationArgumentsCount;
    std::vector<double> m_operationData; // share data

    // OperationPerformer m_operationPerformer; // shared data
};
