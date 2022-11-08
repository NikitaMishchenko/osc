#pragma once

#include <fstream>

#include <vector>

/*class OutFile
{
public:
    OutFile(const std::string &fileName)
    {
        m_fout.open(fileName);
    }
    
    ~OutFile()
    {
        if(m_fout.is_open())
            m_fout.close();
    }

private:
    std::ofstream m_fout;
};*/

// todo make append to file
template<class T>
bool writeToFile(std::ofstream fout, const T& outputData)
{
    if (!fout.is_open())
        return false;
    
    fout << outputData;

    return true;
}

template<class T>
bool readFromFile(const std::ifstream fin, T& inputData)
{
    if (!fin.is_open())
        return false;

    if (bool(fin.eof))
        return false;
    
    fin >> inputData;

    return true;
}