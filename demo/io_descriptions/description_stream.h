#pragma once

#include <string>
#include <sstream>

#include <boost/filesystem.hpp>



class DescriptionStream : public std::stringstream
{
public:
    DescriptionStream(boost::filesystem::path path, std::string fileName = "") : m_descriptionName(fileName)
    {
        if (!m_descriptionName.empty())
            m_descriptionName += ".description";
        else
            m_descriptionName = "description";

        m_descriptionFile = path / m_descriptionName;
    }

    virtual ~DescriptionStream()
    {
        std::ofstream fout(m_descriptionFile.string());

        fout << this->str();
    }

private:
    std::string m_descriptionName;
    boost::filesystem::path m_descriptionFile;

    std::ofstream fout;
};