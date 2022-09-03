#pragma once

#include <string>
#include <strstream>
#include <fstream>
#include <iostream>

namespace oscillation_helpers
{

    enum FileType
    {
        DEFAULT,
        TIME_ANGLE,
        TIME_ANGLE_DANGLE,
        TIME_ANGLE_DANGLE_DDANGLE
    };

// todo rewrite
/**
*   когда есть больше 2 точек в строке
*/
bool isOscillationFile(const std::string& fileName)
    {
        std::ifstream fin(fileName);

        std::string str1;

        std::getline(fin, str1);

        std::cout << "get first string: " << str1 << "\n";

        int findOne = 0;

        size_t pos = 0;
        while (1)
        {
            pos = str1.find('.', pos);
            pos++;

            if (std::string::npos == pos || pos == 0)
            {
                break;
            }

            findOne++;

            if (findOne > 2)
            {
                std::cout << "probably osillation file\n";
                return true;
            }
        }

        fin.close();

        std::cout << "probably raw file\n";

        return false;
    }

    /*FileType typeOfOscillationFile(const std::string& fileName)
    {
        std::vector<std::string> tokens;
        std::istringstream   mySstream(fileName);
        std::string         temp;

        const char delim = ' ';

        while (getline(mySstream, temp, delim))
        {
            tokens.push_back( temp );
        }

        //return tokens;
        return DEFAULT;
    }*/


} // namespace oscillation
