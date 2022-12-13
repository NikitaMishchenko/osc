#include <gtest/gtest.h>

#include <cmath>

#include "oscillation/wt_oscillation.h"


TEST(TestOfTest, test1)
{
    ASSERT_TRUE(true);
}

TEST(Test, Freq)
{
    const size_t size = 1000;
    const double sampleSize = 0.001;
    const double w = 10; 

    std::vector<double> time(size);
    std::vector<double> signal(size);
    
    double tmpSignalValue = 0;

    for (int i = 0; i < size; ++i)
    {
        time.push_back(i*sampleSize);

        tmpSignalValue = sin(time.at(i)*w);
        signal.push_back(tmpSignalValue);
    }
    
    


}