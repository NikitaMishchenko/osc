#pragma once

#include <iostream>

class ProcessorIo
{
public:
    ProcessorIo(std::stringstream& descriptionStream) : m_descriptionStream(descriptionStream)
    {}

    virtual ~ProcessorIo()
    {
        m_descriptionStream << "#####################################\n\n";
        m_descriptionStream << "#####################################\n";
    }
    

protected:
    std::stringstream& m_descriptionStream;
private:
};

