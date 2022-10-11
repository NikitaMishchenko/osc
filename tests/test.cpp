#pragma once

#include <utility>
#include <string>
#include <vector>

#include "../src/basic_procedures.h"
#include "../src/errcodes.h"

/** TODO
    make an .cpp and make tests based on GTEST
*/

namespace
{

    template <class A>
    bool EXPECTED_EQ(A f, A s)
    {
        return (f == s);
    }

    template <class A>
    bool EXPECTED_NE(A f, A s)
    {
        return !EXPECTED_EQ(f, s);
    }
}

// todo performing queue ???

int makeAllTests()
{
    std::cout << "testing stated. Performing...\n";

    std::vector<std::pair<bool, uint16_t>> result;
    result.reserve(5);

    int counter = 0;

    result.emplace_back(EXPECTED_EQ(basic_procedures::FAIL, basic_procedures::performProcedurePeriods("empty_osc", std::string(), std::vector<double>())),
                        counter);

    counter++;

    std::cout << "Tests were performed!\n";
    std::cout << "results:\nnumber\tstatus\n";
    for (auto k : result)
    {
        std::cout << k.second << "\t"
                  << (k.first ? "SUCESS" : "FAIL") << "\n"
                  << "______________\n";
    }
    // basic_procedures::performProcedurePeriods("empty_osc", std::string(), std::vector<double>());

    return 0;
}

int main(int argc, char **argv)
{
    return makeAllTests();
}