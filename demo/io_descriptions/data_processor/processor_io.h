#pragma once

#include <iostream>
#include <vector>
#include <memory>

namespace io 
{
    class ProcessorIo
    {
    public:
        ProcessorIo(std::vector<std::shared_ptr<std::stringstream> > descriptionStreamVector) : m_descriptionStreamVector(descriptionStreamVector)
        {}

        virtual ~ProcessorIo()
        {
            for (auto& descriptionStream : m_descriptionStreamVector)
                if (!descriptionStream->str().empty())
                {
                    *descriptionStream << "#####################################\n\n";
                    *descriptionStream << "#####################################\n";
                }
        }
        

    protected:
        mutable std::vector<std::shared_ptr<std::stringstream>> m_descriptionStreamVector;
    private:
    };
}
