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

TEST(TestApproximation, Approximation1)
{
    std::vector<double> dataX;
    std::vector<double> dataY;
    size_t N = 100;

    double A = 50;
    double B = 1.0;
    double lambda = 1.5;

    for (int i = 0; i < N; ++i)
    {
        dataX.resize(N);
        dataY.resize(N);

        double dt = 0.1;

        for (int i = 0; i < dataX.size(); ++i)
        {
            dataX.at(i) = dt * i;
            dataY.at(i) = (B + A * exp(-1.0*lambda * i * dt));
        }
    }

    /*double dx, dy;
    std::ifstream fin("approx_1_1");
    for (int i = 0; i < 4; i++)
    {
        fin >> dx >> dy;
        dataX.push_back(dx);
        dataY.push_back(dy);
    }*/

    AngleHistory angleHistory(dataX, dataY);

    int errCode = 1;
    approximation::nonlinnear::ApproximationResult approximationResult;

    std::tie(errCode, approximationResult) = approximation::nonlinnear::approximate(angleHistory.getTime(), angleHistory.getAngle());

    std::ofstream foutIn("approxTest_input1");
    
    for (size_t i = 0; i < dataX.size(); i++)
    {
        foutIn << dataX.at(i) << "\t" << dataY.at(i) << "\n";
    }

    std::ofstream foutOut("approxTest_out1");
    foutOut << approximationResult.A << "\t" << approximationResult.B << "\t" << approximationResult.lambda << "\n";

    ASSERT_TRUE(std::abs(A - approximationResult.A) < 0.0001);
    ASSERT_TRUE(std::abs(B - approximationResult.B) < 0.0001);
    ASSERT_TRUE(std::abs(lambda - approximationResult.lambda) < 0.0001);
}

TEST(TestApproximation, Approximation2)
{
    std::vector<double> dataX;
    std::vector<double> dataY;
    size_t N = 100;

    double A = 50;
    double B = 0.0;
    double lambda = 1.5;

    for (int i = 0; i < N; ++i)
    {
        dataX.resize(N);
        dataY.resize(N);

        double dt = 0.1;

        for (int i = 0; i < dataX.size(); ++i)
        {
            dataX.at(i) = dt * i;
            dataY.at(i) = (B + A * exp(-1.0*lambda * i * dt));
        }
    }

    AngleHistory angleHistory(dataX, dataY);

    int errCode = 1;
    approximation::nonlinnear::ApproximationResult approximationResult;

    std::tie(errCode, approximationResult) = approximation::nonlinnear::approximate(angleHistory.getTime(), angleHistory.getAngle());

    std::ofstream foutIn("approxTest_input2");
    
    for (size_t i = 0; i < dataX.size(); i++)
    {
        foutIn << dataX.at(i) << "\t" << dataY.at(i) << "\n";
    }

    std::ofstream foutOut("approxTest_out2");
    foutOut << approximationResult.A << "\t" << approximationResult.B << "\t" << approximationResult.lambda << "\n";

    ASSERT_TRUE(std::abs(A - approximationResult.A) < 0.0001);
    ASSERT_TRUE(std::abs(B - approximationResult.B) < 0.0001);
    ASSERT_TRUE(std::abs(lambda - approximationResult.lambda) < 0.0001);
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
                                  0.0,
                                  50000,
                                  0.0001,
                                  {50, 0.1});

        functionProbeDataVector.push_back(functionProbeData);

        ///
        auto function1 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) * sin(coeff.at(0) * argument) * exp(coeff.at(2) * argument);
        };

        functionProbeData.setData(function1,
                                  0.0,
                                  50000,
                                  0.0001,
                                  {50, 1, -1});

        functionProbeDataVector.push_back(functionProbeData);

        ///
        auto function2 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) * sin(coeff.at(0) * argument) * exp(coeff.at(2) * argument);
        };

        functionProbeData.setData(function2,
                                  0.0,
                                  50000,
                                  0.0001,
                                  {50, 1, -2});

        functionProbeDataVector.push_back(functionProbeData);

        ///
        auto function3 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) * sin(coeff.at(0) * argument) * exp(coeff.at(2) * argument);
        };

        functionProbeData.setData(function3,
                                  0.0,
                                  50000,
                                  0.0001,
                                  {50, 1, -3});

        functionProbeDataVector.push_back(functionProbeData);
    }

    std::cout << "functionProbeDataVector.size() = " << functionProbeDataVector.size() << "\n";

    bool isOk = false;
    std::vector<Function> functionVector;

    std::tie(isOk, functionVector) = functionGenerator.actOnData(functionProbeDataVector);

    ASSERT_TRUE(isOk);

    // todo check actial functuanallity

    for (int i = 0; i < functionVector.size(); i++) // 
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
                                                      std::make_shared<Model>(),
                                                      20);

            std::string indexForScript = std::to_string(i);
            pitchDynamicMomentum.setHiddenIndex(indexForScript);

            std::vector<double> staticR;
            std::vector<PitchMomentumBasic> basicR;
            std::vector<double> dynamicR;
            std::vector<AngleAmplitudeBase> amplitude;

            {
                ASSERT_TRUE(pitchDynamicMomentum.calculateEqvivalentDampingCoefficients(METHOD_2));

                std::tie(staticR, basicR, dynamicR, amplitude) = pitchDynamicMomentum.getData();

                std::string fileNamePlotApprox("plotApprox_" + indexForScript);
                std::ofstream plotApproxFile(fileNamePlotApprox);
                plotApproxFile << pitchDynamicMomentum.getPlottScript()
                               << ", \"test_" << std::to_string(i) << "\" using 1:2 with linespoints\n";

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
                {
                    fout2 << basicR.at(i).time << "\t" << basicR.at(i).angle << "\t" << basicR.at(i).momentum << "\t" 
                          << basicR.at(i).timeI << "\t" << basicR.at(i).angleI << "\t" << basicR.at(i).momentum << "\t"
                          << basicR.at(i).timeF << "\t" << basicR.at(i).angleF << "\t" << basicR.at(i).momentum << "\n";

                    std::cout << basicR.at(i).time << "\t" << basicR.at(i).angle << "\t" << basicR.at(i).momentum << "\t"
                              << basicR.at(i).timeI << "\t" << basicR.at(i).angleI << "\t" << basicR.at(i).momentum << "\t"
                              << basicR.at(i).timeF << "\t" << basicR.at(i).angleF << "\t" << basicR.at(i).momentum << "\n";
                }
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

// plot "basic_1" using 1:2 with linespoints, "test_1" using 1:2 with linespoints, 0.97*exp(-1.1*x)+0.0, "approx_1_0" using 1:2
// f(x) = A*exp(-1.0*l*x)+B
// fit f(x) "approx_1_0" using 1:2 via A,l,B