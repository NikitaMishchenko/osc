#include <gtest/gtest.h>

#include <cmath>

#include <iostream>
#include <fstream>

#include "oscillation/wt_oscillation.h"

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

std::tuple<std::vector<double>, std::vector<double>>
getFuncionResult(double (*function)(double, std::vector<double> ),
                 double time0,
                 size_t length,
                 double dt,
                 std::vector<double> coeff)
{
    std::vector<double> func;
    std::vector<double> time;

    for (size_t i = 0; i < length; i++)
    {
        double t = i * dt;

        time.push_back(time0 + t);
        func.push_back(function(t, coeff));
    }

    return std::make_tuple(time, func);
}

struct FunctionProbeData
{
    FunctionProbeData() : m_fileName(""),
                          m_time0(0),
                          m_length(0),
                          m_dt(0)
    {
    }

    FunctionProbeData(const std::string &fileName,
                      double time0,
                      size_t length,
                      double dt,
                      std::vector<double> coeff) : m_fileName(fileName),
                                                   m_time0(time0),
                                                   m_length(length),
                                                   m_dt(dt),
                                                   m_coeff(coeff)

    {
    }

    void setData(const std::string &fileName,
                 double time0,
                 size_t length,
                 double dt,
                 std::vector<double> coeff)
    {
        m_fileName = fileName;
        m_time0 = time0;
        m_length = length;
        m_dt = dt;
        m_coeff = coeff;
    }

    std::string m_fileName;

    double m_time0;
    size_t m_length;
    double m_dt;
    std::vector<double> m_coeff;
};

void actAndSaveData(const FunctionProbeData &functionProbeData,
                    double (*function)(double, std::vector<double>))
{
    std::vector<double> func;
    std::vector<double> time;

    std::tie(time, func) = getFuncionResult(function,
                                            functionProbeData.m_time0,
                                            functionProbeData.m_length,
                                            functionProbeData.m_dt,
                                            functionProbeData.m_coeff);

    std::ofstream fout(functionProbeData.m_fileName);

    for (size_t i = 0; i < func.size(); i++)
    {
        fout << time.at(i) << "\t"
             << func.at(i) << "\n";
    }
}

TEST(TestOnGeneratedData, test_gen_data)
{
    FunctionProbeData functionProbeData;

    {
        functionProbeData.setData("test1", 2.0, 100, 0.1, {0.1, 3});

        auto function1 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(0) / argument;
        };

        actAndSaveData(functionProbeData, function1);

        functionProbeData.setData("test2", 1.0, 100, 0.1,  {0.2, 3});

        auto function2 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(0) / argument;
        };

        actAndSaveData(functionProbeData, function2);
    }
}
