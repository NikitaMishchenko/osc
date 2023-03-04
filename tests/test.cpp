#include <gtest/gtest.h>

#include <cmath>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "oscillation/wt_oscillation.h"
#include "utils/function_generator/function_generator.h"

TEST(TestOfTest, test1)
{
    ASSERT_TRUE(true);
}

TEST(Test, Freq)
{
    const size_t size = 2000;
    const double sampleSize = 0.1;
    const double w = 0.01;

    std::vector<double> time;
    std::vector<double> signal;

    double tmpSignalValue = 0;

    for (int i = 0; i < size; ++i)
    {
        time.push_back(i * sampleSize);

        tmpSignalValue = sin(time.at(i) * w * 2 * M_PI);
        signal.push_back(tmpSignalValue);
    }

    AngleHistory angleHistory(time, signal);

    angleHistory.info();
    // angleHistory.write("testAH");

    WtOscillation wt(angleHistory);

    wt.calcAngleAmplitudeIndexes();

    double wCalculated = wt.getW();

    std::cout << "wCalculated: " << wCalculated << "\n";

    ASSERT_TRUE(abs(w - wCalculated) / w < 0.05); // todo cash presision
}

TEST(TestOnGeneratedData, test_gen_data)
{
    function_generator::FunctionGenerator functionGenerator;

    std::vector<function_generator::FunctionProbeData> functionProbeDataVector;

    auto function1 = [](double argument, std::vector<double> coeff)
    {
        return coeff.at(0) / argument;
    };

    {
        function_generator::FunctionProbeData functionProbeData;

        auto function1 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(0) / argument;
        };

        functionProbeData.setData(function1,
                                  1.0,
                                  50,
                                  0.1,
                                  {0.1});

        functionProbeDataVector.push_back(functionProbeData);

        functionProbeData.setData(function1,
                                  1.0,
                                  50,
                                  0.1,
                                  {0.2});

        functionProbeDataVector.push_back(functionProbeData);

        auto function2 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(0)*argument + coeff.at(1);
        };

        functionProbeData.setData(function2,
                                  1.0,
                                  50,
                                  0.1,
                                  {-0.2, 1});

        functionProbeDataVector.push_back(functionProbeData);
    }

    std::cout << "functionProbeDataVector.size() = " << functionProbeDataVector.size() << "\n";

    bool isOk = false;
    std::vector<Function> functionVector;

    std::tie(isOk, functionVector) = functionGenerator.actOnData(functionProbeDataVector);

    ASSERT_TRUE(isOk);
}
