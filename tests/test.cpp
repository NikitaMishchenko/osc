#include <gtest/gtest.h>

#include <cmath>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

#include "oscillation/wt_oscillation.h"
#include "utils/function_generator/function_generator.h"
#include "analize_coefficients/specific/pitch_dynamic_momentum.h"

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
/*
TEST(TestOnGeneratedData, testBasicsInitialisation)
{
    std::vector<double> x;
    std::vector<double> y;

    {
        AngleHistory angleHistory0(x, y);

        Oscillation oscillation(angleHistory0);

        PitchDynamicMomentum pitchDynamicMomentum(std::make_shared<std::vector<double>>(oscillation.getTime()),
                                                  std::make_shared<std::vector<double>>(oscillation.getAngle()),
                                                  std::make_shared<std::vector<double>>(oscillation.getDangle()),
                                                  std::make_shared<std::vector<double>>(oscillation.getDdangle()),
                                                  std::make_shared<wt_flow::Flow>(),
                                                  std::make_shared<Model>());

        ASSERT_FALSE(pitchDynamicMomentum.calcuatePitchStaticMomentum());
    }

    for (int i = 0; i < 100; i++)
    {
        x.push_back(0.1 * i);
        y.push_back(0.1);
    }

    {
        AngleHistory angleHistory0(x, y);

        Oscillation oscillation(angleHistory0);

        PitchDynamicMomentum pitchDynamicMomentum(std::make_shared<std::vector<double>>(oscillation.getTime()),
                                                  std::make_shared<std::vector<double>>(oscillation.getAngle()),
                                                  std::make_shared<std::vector<double>>(oscillation.getDangle()),
                                                  std::make_shared<std::vector<double>>(oscillation.getDdangle()),
                                                  std::make_shared<wt_flow::Flow>(),
                                                  std::make_shared<Model>());

        ASSERT_TRUE(pitchDynamicMomentum.calcuatePitchStaticMomentum());
    }
}*/

TEST(TestOnGeneratedData, test_gen_data)
{
    function_generator::FunctionGenerator functionGenerator;

    std::vector<function_generator::FunctionProbeData> functionProbeDataVector;

    {
        function_generator::FunctionProbeData functionProbeData;

        // consider exception inside function
        auto function0 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) / abs(argument) * sin(coeff.at(0) * argument);
        };

        functionProbeData.setData(function0,
                                  1.0,
                                  500,
                                  0.01,
                                  {10, 0.1});

        functionProbeDataVector.push_back(functionProbeData);

        ///
        auto function1 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) * sin(coeff.at(0) * argument) * exp(coeff.at(2) * argument);
        };

        functionProbeData.setData(function1,
                                  1.0,
                                  500,
                                  0.01,
                                  {10, 1, -0.1});

        functionProbeDataVector.push_back(functionProbeData);

        ///
        auto function2 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) * sin(coeff.at(0) * argument) * exp(coeff.at(2) * argument);
        };

        functionProbeData.setData(function2,
                                  1.0,
                                  500,
                                  0.01,
                                  {10, 1, -0.2});

        functionProbeDataVector.push_back(functionProbeData);

        ///
        auto function3 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) * sin(coeff.at(0) * argument) * exp(coeff.at(2) * argument);
        };

        functionProbeData.setData(function3,
                                  1.0,
                                  500,
                                  0.01,
                                  {10, 1, -0.3});

        functionProbeDataVector.push_back(functionProbeData);
    }

    std::cout << "functionProbeDataVector.size() = " << functionProbeDataVector.size() << "\n";

    bool isOk = false;
    std::vector<Function> functionVector;

    std::tie(isOk, functionVector) = functionGenerator.actOnData(functionProbeDataVector);

    ASSERT_TRUE(isOk);

    // todo check actial functuanallity

    for (int i = 0; i < functionVector.size(); i++) // functionVector.size()
    {
        AngleHistory angleHistory(functionVector.at(i).getDomain(), functionVector.at(i).getCodomain());

        Oscillation oscillation(angleHistory);

        std::cout << oscillation.getTime().size() << "\t" << oscillation.getAngle().size() << "\t" << oscillation.getDangle().size() << "\t" << oscillation.getDangle().size() << "\n";

        {
            PitchDynamicMomentum pitchDynamicMomentum(std::make_shared<std::vector<double>>(oscillation.getTime()),
                                                      std::make_shared<std::vector<double>>(oscillation.getAngle()),
                                                      std::make_shared<std::vector<double>>(oscillation.getDangle()),
                                                      std::make_shared<std::vector<double>>(oscillation.getDdangle()),
                                                      std::make_shared<wt_flow::Flow>(),
                                                      std::make_shared<Model>());
            // pitchDynamicMomentum.setHiddenIndex(i);

            std::vector<double> staticR;
            std::vector<double> basicR;
            std::vector<double> dynamicR;
            std::vector<AngleAmplitudeBase> amplitude;

            {
                ASSERT_TRUE(pitchDynamicMomentum.calculateEqvivalentDampingCoefficients(METHOD_2));

                std::tie(staticR, basicR, dynamicR, amplitude) = pitchDynamicMomentum.getData();

                std::ofstream foutb("test_" + std::to_string(i));
                for (int i = 0; i < oscillation.size(); i++)
                    foutb << oscillation.getTime(i) << "\t"
                          << oscillation.getAngle(i) << "\t"
                          << oscillation.getDangle(i) << "\t"
                          << oscillation.getDdangle(i) << "\n";

                std::ofstream fout1("static_" + std::to_string(i));
                for (int i = 0; i < staticR.size(); i++)
                    fout1 << staticR.at(i) << "\n";

                std::ofstream fout2("basic_" + std::to_string(i));
                for (int i = 0; i < basicR.size(); i++)
                    fout2 << basicR.at(i) << "\n";

                std::ofstream fout3("dynamic_" + std::to_string(i));
                for (int i = 0; i < dynamicR.size(); i++)
                    fout3 << dynamicR.at(i) << "\n";

                std::ofstream fout4("amplitude_" + std::to_string(i));
                for (int i = 0; i < amplitude.size(); i++)
                    fout4 << amplitude.at(i).m_amplitudeTime << "\t"
                          << amplitude.at(i).m_amplitudeAngle << "\t"
                          << amplitude.at(i).m_amplitudeIndexesFromInitialAngle << "\n";
            }
        }
    }
}
