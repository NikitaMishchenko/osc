#pragma once

#include <istream>
#include <queue>

#include "operation_type_parsers.h"

class OperationsQueue
{
public:
    OperationsQueue(const std::string &fileName)
    {
        std::cout << "constucting OperationQueue() loading config file: \"" << fileName << "\"\n";  

        std::ifstream fin(fileName);

        if (!fin.is_open())
            throw std::string("OperationQueue cant load file:" + fileName);

        OperationType type;

        do
        {
            type.clear();

            if(!type.loadFromSource(fin))
                break;

            m_operations.push(type.getOperationPerformer());

        }  while (type.getOperationType() != Operations::END);

        std::cout << "queue loaded\n";
    }

    void doWork()
    {
        std::cout << "performing queue Operations\n";
        
        while (!m_operations.empty() || m_operations.front().getOperationType() != Operations::END)
        {
            m_operations.front().perform();
            m_operations.pop();
        }
    }

private:
    std::queue<OperationPerformer> m_operations;
};