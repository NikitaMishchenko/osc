#pragma once

#include <string>
#include <fstream>
#include <iostream>

namespace oscillation_helpers
{

    // todo rewrite
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
} // namespace oscillation
