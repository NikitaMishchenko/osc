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

// all data assumed being same sizes
template <class T>
bool writeToFile(std::ofstream &fout, const std::vector<T> &outputData)
{
    if (!fout.is_open())
        return false;

    for (int i = 0; i < outputData.at(0).size(); ++i)
    {
        for (const auto &one : outputData)
        {
            fout << one.at(i) << "\t";
        }

        fout << "\n";
    }

    return true;
}

template <class T>
bool writeToFile(std::ofstream &fout, const T &outputData)
{
    if (!fout.is_open())
        return false;

    fout << outputData;

    return true;
}

template <class T>
bool readFromFile(const std::ifstream fin, T &inputData)
{
    if (!fin.is_open())
        return false;

    if (bool(fin.eof))
        return false;

    fin >> inputData;

    return true;
}