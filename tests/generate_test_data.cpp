#include <gtest/gtest.h>

#include <cmath>

#include <iostream>
#include <fstream>


TEST(TestOnGeneratedData, test_gen_data)
{
    std::ofstream fout("./data/test1");



    fout.close();

    ASSERT_FALSE(true);
}
