#include <gtest/gtest.h>

#include <cmath>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>

#include "oscillation/wt_oscillation.h"
#include "utils/function_generator/function_generator.h"
#include "analize_coefficients/specific/pitch_dynamic_momentum.h"
#include "gnuplot/gnuplot_wrapper.h"

TEST(TestOfTest, test1)
{
    ASSERT_TRUE(true);
}

TEST(TestGnuplot, BasicTest)
{
    {
        const size_t size = 100;

        gnuplot::Gnuplot1d gnuplot1d;
        std::vector<double> arg, func;

        {
            arg.clear();
            func.clear();

            arg.reserve(size);
            func.reserve(size);

            for (size_t i = 0; i < size; i++)
            {
                arg.push_back(i * 0.1);
                func.push_back(10*arg.back());
            }

            gnuplot::DataFunction dataFunction(arg, func);

            dataFunction.setLineColor(gnuplot::RED);
            dataFunction.setLineType(gnuplot::LINES);

            gnuplot1d.addDataToPlot(dataFunction);
        }

        {
            arg.clear();
            func.clear();

            arg.reserve(size);
            func.reserve(size);

            for (size_t i = 0; i < size; i++)
            {
                arg.push_back(i * 0.1);
                func.push_back(arg.back() * arg.back() + arg.back());
            }

            gnuplot::DataFunction dataFunction(arg, func);

            dataFunction.setLineColor(gnuplot::BLACK);
            dataFunction.setLineType(gnuplot::LINES_POINTS);
            
            gnuplot1d.addDataToPlot(dataFunction);
        }

        gnuplot1d.setPlotTitle("testTitle");
        gnuplot1d.act1dVector();
    }
}
/*
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
            dataY.at(i) = (B + A * exp(-1.0 * lambda * i * dt));
        }
    }

    AngleHistory angleHistory(dataX, dataY);

    int errCode = 1;
    approximation::nonlinnear::ApproximationResult approximationResult;
    std::string msg;

    std::tie(errCode, approximationResult, msg) = approximation::nonlinnear::approximateAndInfo(angleHistory.getTime(), angleHistory.getAngle());

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
            dataY.at(i) = (B + A * exp(-1.0 * lambda * i * dt));
        }
    }

    AngleHistory angleHistory(dataX, dataY);

    int errCode = 1;
    approximation::nonlinnear::ApproximationResult approximationResult;
    std::string msg;

    std::tie(errCode, approximationResult, msg) = approximation::nonlinnear::approximateAndInfo(angleHistory.getTime(), angleHistory.getAngle());

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
}

TEST(TestPitchMomentumBasicVector, atTimeTest)
{
    std::vector<PitchMomentumBasic> base;
    PitchMomentumBasic b;
    const double step = 0.1;
    size_t sizeOfTest = 3;

    for (size_t i = 0; i < sizeOfTest; i++)
    {
        b.time = i * step;

        base.push_back(b);
    }

    double timeLastInRange = base.back().time;
    double timeOutOfRangeUpper = timeLastInRange + step;
    ASSERT_EQ(timeLastInRange, helpers::getAtTime(base, timeOutOfRangeUpper).time);

    double timeFirstInRange = base.front().time;
    double timeOutOfRangeBelow = timeFirstInRange - step;
    ASSERT_EQ(timeFirstInRange, helpers::getAtTime(base, timeOutOfRangeBelow).time);

    ASSERT_TRUE(base.size() > 2);
    double timeInRange = (base.begin() + 1)->time;

    ASSERT_EQ(timeInRange, helpers::getAtTime(base, timeInRange).time);

    double timeInRangeMoreThenOne = timeInRange + 0.1 * step;
    ASSERT_EQ(timeInRange, helpers::getAtTime(base, timeInRangeMoreThenOne).time);
}

TEST(TestOnGeneratedData, test_gen_data)
{
    function_generator::FunctionGenerator functionGenerator;

    std::vector<function_generator::FunctionProbeData> functionProbeDataVector;

    std::stringstream totalInfo;

    {
        function_generator::FunctionProbeData functionProbeData;

        std::vector<double> coeff;
        double time0 = 0.0, dt = 0.0001;
        int length = 50000;

        // consider exception inside function
        auto function0 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) / abs(argument) * sin(coeff.at(0) * argument);
        };
        {
            coeff = {50, 0.5};
            std::stringstream descriptionFunc;

            descriptionFunc << "f(arg) = " << coeff.at(1) << " * sin(" << coeff.at(0) << " * arg) " << "/abs(arg)\n";

            totalInfo << "setting probe function:\n\t" << descriptionFunc.str() << "\n";

            totalInfo << "setting function coefficients: \n"
                     << "\ttime0 = " << time0 << "\n"
                     << "\tlength0 = " << length << "\n"
                     << "\tdt = " << dt << "\n"
                     << "\tcoefficients = ";
            for (const auto& c : coeff)
                totalInfo  << c << ", ";
            totalInfo << "\n______________________\n\n";

            functionProbeData.setData(function0,
                                      descriptionFunc.str(),
                                      time0,
                                      length,
                                      dt,
                                      coeff);

            functionProbeDataVector.push_back(functionProbeData);
        }

        ///
        auto function1 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) * sin(coeff.at(0) * argument) * exp(coeff.at(2) * argument);
        };
        {
            std::vector<double> coeff = {50, 1, -1};
            std::stringstream descriptionFunc;

            descriptionFunc << "f(arg) = " << coeff.at(1) << " * sin(" << coeff.at(0) << " * arg) * exp(" << coeff.at(2) << "*arg)\n";

            totalInfo << "setting probe function:\n\t" << descriptionFunc.str() << "\n";

            totalInfo << "setting function coefficients: \n"
                     << "\ttime0 = " << time0 << "\n"
                     << "\tlength0 = " << length << "\n"
                     << "\tdt = " << dt << "\n"
                     << "\tcoefficients = ";
            for (const auto& c : coeff)
                totalInfo  << c << ", ";
            totalInfo << "\n______________________\n\n";

            functionProbeData.setData(function1,
                                      descriptionFunc.str(),
                                      time0,
                                      length,
                                      dt,
                                      coeff);

            functionProbeDataVector.push_back(functionProbeData);
        }

        ///
        auto function2 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) * sin(coeff.at(0) * argument) * exp(coeff.at(2) * argument);
        };
        {
            std::vector<double> coeff = {50, 1, -2};
            std::stringstream descriptionFunc;

            descriptionFunc << "f(arg) = " << coeff.at(1) << " * sin(" << coeff.at(0) << " * arg) * exp(" << coeff.at(2) << "*arg)\n";

            totalInfo << "setting probe function:\n\t" << descriptionFunc.str() << "\n";

            totalInfo << "setting function coefficients: \n"
                     << "\ttime0 = " << time0 << "\n"
                     << "\tlength0 = " << length << "\n"
                     << "\tdt = " << dt << "\n"
                     << "\tcoefficients = ";
            for (const auto& c : coeff)
                totalInfo  << c << ", ";
            totalInfo << "\n______________________\n\n";

            functionProbeData.setData(function2,
                                      descriptionFunc.str(),
                                      time0,
                                      length,
                                      dt,
                                      coeff);

            functionProbeDataVector.push_back(functionProbeData);
        }

        ///
        auto function3 = [](double argument, std::vector<double> coeff)
        {
            return coeff.at(1) * sin(coeff.at(0) * argument) * exp(coeff.at(2) * argument);
        };
        {
            std::vector<double> coeff = {50, 1, -3};
            std::stringstream descriptionFunc;

            descriptionFunc << "f(arg) = " << coeff.at(1) << " * sin(" << coeff.at(0) << " * arg) * exp(" << coeff.at(2) << "*arg)\n";

            totalInfo << "setting probe function:\n\t" << descriptionFunc.str() << "\n";

            totalInfo << "setting function coefficients: \n"
                     << "\ttime0 = " << time0 << "\n"
                     << "\tlength0 = " << length << "\n"
                     << "\tdt = " << dt << "\n"
                     << "\tcoefficients = ";
            for (const auto& c : coeff)
                totalInfo  << c << ", ";
            totalInfo << "\n______________________\n\n";

            functionProbeData.setData(function3,
                                      descriptionFunc.str(),
                                      time0,
                                      length,
                                      dt,
                                      coeff);

            functionProbeDataVector.push_back(functionProbeData);
        }

        totalInfo << "\n";
    }

    totalInfo << "functionProbeDataVector.size() = " << functionProbeDataVector.size() << "\n";

    bool isOk = false;
    std::vector<Function> functionVector;

    std::tie(isOk, functionVector) = functionGenerator.actOnData(functionProbeDataVector);

    totalInfo << functionGenerator.getProcInfo() << "\n";

    ASSERT_TRUE(isOk);

    // todo check actial functuanallity

    const int PERIODS_IN_CALC = 2;
    std::string fileNameBaseTestData = "test_";
    std::string fileNameBasicAmplitude = "amplitude_";
    std::string fileNameBasicPlotApprox = "plotApprox_";
    std::string fileNameBasicResultStatic = "static_";
    std::string fileNameBasicBasicCoeff = "basic_";
    std::string fileNameBasicDynamicCoeff = "dynamic_";

    for (int i = 0; i < functionVector.size(); i++)
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
                                                      PERIODS_IN_CALC);

            std::string indexString = std::to_string(i);
            pitchDynamicMomentum.setHiddenIndex(indexString);

            std::vector<PitchMomentumBasic> staticR;
            std::vector<PitchMomentumBasic> basicR;
            std::vector<PitchMomentumBasic> dynamicR;
            std::vector<AngleAmplitudeBase> amplitude;

            {
                ASSERT_TRUE(pitchDynamicMomentum.calculateEqvivalentDampingCoefficients());

                totalInfo << pitchDynamicMomentum.getProceduresHistory();

                std::tie(staticR, basicR, dynamicR, amplitude) = pitchDynamicMomentum.getData();

                {
                    std::ofstream fout1(fileNameBasicResultStatic + indexString);
                    for (int i = 0; i < staticR.size(); i++)
                        fout1 << staticR.at(i).time << "\t"
                              << staticR.at(i).angle << "\t"
                              << staticR.at(i).momentum << "\n";
                }

                {
                    std::ofstream fout2(fileNameBasicBasicCoeff + indexString);
                    for (int i = 0; i < basicR.size(); i++)
                        fout2 << basicR.at(i).time << "\t" << basicR.at(i).angle << "\t" << basicR.at(i).momentum << "\t"
                              << basicR.at(i).timeI << "\t" << basicR.at(i).angleI << "\t" << basicR.at(i).momentum << "\t"
                              << basicR.at(i).timeF << "\t" << basicR.at(i).angleF << "\t" << basicR.at(i).momentum << "\n";
                }

                {
                    std::ofstream fout3(fileNameBasicDynamicCoeff + indexString);
                    for (int i = 0; i < dynamicR.size(); i++)
                        fout3 << dynamicR.at(i).time << "\t"
                              << dynamicR.at(i).angle << "\t"
                              << dynamicR.at(i).dangle << "\t"
                              << dynamicR.at(i).momentum << "\n";
                }

                {
                    std::ofstream fout4(fileNameBasicAmplitude + indexString);
                    for (int i = 0; i < amplitude.size(); i++)
                        fout4 << amplitude.at(i).m_amplitudeTime << "\t"
                              << amplitude.at(i).m_amplitudeAngle << "\t"
                              << amplitude.at(i).m_amplitudeIndexesFromInitialAngle << "\n";
                }

                {
                    std::string fileNameTestData = fileNameBaseTestData + indexString;

                    std::ofstream foutb(fileNameTestData);
                    for (int i = 0; i < oscillation.size(); i++)
                        foutb << oscillation.getTime(i) << "\t"
                              << oscillation.getAngle(i) << "\t"
                              << oscillation.getDangle(i) << "\t"
                              << oscillation.getDdangle(i) << "\n";

                    std::ofstream plotApproxFile(fileNameBasicPlotApprox + indexString);
                    plotApproxFile << pitchDynamicMomentum.getPlottScript()
                                   << ", \"" << fileNameTestData << "\" using 1:2 with linespoints\n";
                }
            }
        }
    }

    {
        std::ofstream foutDescription("description");
        std::stringstream streamDescription;
        streamDescription
            << "\n******************************************************************************\n"
            << "\n******************************************************************************\n"
            << "Результаты обработки, где i - номер теста, j - номер части исходных данных по периодам:\n"
            << "использовано периодов для рассчета: " << PERIODS_IN_CALC << "\n"
            << "\t" << fileNameBasicPlotApprox << " - скрипт построения тестовых данных к аппроксимации в программе Gnuplot (есть для Linux и Windows) (Использовать команду load '" << fileNameBasicPlotApprox << "_i_j'),\n"
            << "\t" << fileNameBasicBasicCoeff << "i - coffDampEqv = -2I*lambda(average Ampl)*V/q/s/l/l [time, angle, coeff, timeInitial, angleInitial, momentum, timeFinal, angleFinal, momentum]\n"
            << "\t" << fileNameBasicResultStatic << "i - результаты обработки Статический коэффициент на максимальных амплитудах [время, угол, ускорение, коэффициент]\n"
            << "\t" << fileNameBasicAmplitude << "i - амплитуда колебаний [время, угол, индекс строки тестовых данных]\n"
            << "\t" << fileNameBasicDynamicCoeff << "i - динамический коэффициент [время, угол, ускорение, коэффициент]\n"
            << "\t" << fileNameBaseTestData << "i - тестовые данные [время, угол, первая произодная угла, вторая производная угла]\n";

        foutDescription << streamDescription.str();
        totalInfo << streamDescription.str();
    }

    std::cout << totalInfo.str();

    totalInfo << "\n\n\nпрочие построения (нет генерации скрипта):\n"
                     "plot \"basic_0\" using 1:2 with linespoints, \"basic_1\" using 1:2 with linespoints, \"basic_2\" using 1:2 with linespoints, \"basic_3\" using 1:2 with linespoints"
                     "plot \"basic_0\" using 1:3 with linespoints, \"basic_1\" using 1:3 with linespoints, \"basic_2\" using 1:3 with linespoints, \"basic_3\" using 1:3 with linespoints\n"
                     "plot \"basic_0\" using 2:3 with linespoints, \"basic_1\" using 2:3 with linespoints, \"basic_2\" using 2:3 with linespoints, \"basic_3\" using 2:3 with linespoints\n"
                     "\n"
                     "plot \"static_0\" using 1:2 with linespoints, \"static_1\" using 1:2 with linespoints, \"static_2\" using 1:2 with linespoints, \"static_3\" using 1:2 with linespoints\n"
                     "plot \"static_0\" using 1:3 with linespoints, \"static_1\" using 1:3 with linespoints, \"static_2\" using 1:3 with linespoints, \"static_3\" using 1:3 with linespoints\n"
                     "\n"
                     "plot \"dynamic_0\" using 1:2 with linespoints, \"dynamic_1\" using 1:2 with linespoints, \"dynamic_2\" using 1:2 with linespoints, \"dynamic_3\" using 1:2 with linespoints\n"
                     "plot \"dynamic_0\" using 1:3 with linespoints, \"dynamic_1\" using 1:3 with linespoints, \"dynamic_2\" using 1:3 with linespoints, \"dynamic_3\" using 1:3 with linespoints\n"
                     "plot \"dynamic_0\" using 1:4 with linespoints, \"dynamic_1\" using 1:4 with linespoints, \"dynamic_2\" using 1:4 with linespoints, \"dynamic_4\" using 1:3 with linespoints\n"
                     "plot \"dynamic_0\" using 2:3 with linespoints, \"dynamic_1\" using 2:3 with linespoints, \"dynamic_2\" using 2:3 with linespoints, \"dynamic_3\" using 2:3 with linespoints\n"
                     "plot \"dynamic_0\" using 2:4 with linespoints, \"dynamic_1\" using 2:4 with linespoints, \"dynamic_2\" using 2:4 with linespoints, \"dynamic_3\" using 2:4 with linespoints\n"
                     "\n";

    std::ofstream foutTotalInfo("log");
    foutTotalInfo << totalInfo.str();
}
*/
// plot "basic_1" using 1:2 with linespoints, "test_1" using 1:2 with linespoints, 0.97*exp(-1.1*x)+0.0, "approx_1_0" using 1:2
// f(x) = A*exp(-1.0*lambda*x)+B
// fit f(x) "approx_1_0" using 1:2 via A,lambda,B

// TODO utomize crating scripts for following plotting routine

// eqvivalent damping Method 1
// time angleAvg momentum
// plot "basic_0" using 1:2 with linespoints, "basic_1" using 1:2 with linespoints, "basic_2" using 1:2 with linespoints, "basic_3" using 1:2 with linespoints
// plot "basic_0" using 1:3 with linespoints, "basic_1" using 1:3 with linespoints, "basic_2" using 1:3 with linespoints, "basic_3" using 1:3 with linespoints
// plot "basic_0" using 2:3 with linespoints, "basic_1" using 2:3 with linespoints, "basic_2" using 2:3 with linespoints, "basic_3" using 2:3 with linespoints

// ok
// pitch static momentum at time = 0 // (Iz * m_ddangle->at(i) - M_fr) / coeff
// time angleAvg momentum
// ! plot "static_0" using 2:3 with linespoints, "static_1" using 2:3 with linespoints, "static_2" using 2:3 with linespoints, "static_3" using 2:3 with linespoints
// plot "static_0" using 1:2 with linespoints, "static_1" using 1:2 with linespoints, "static_2" using 1:2 with linespoints, "static_3" using 1:2 with linespoints
// plot "static_0" using 1:3 with linespoints, "static_1" using 1:3 with linespoints, "static_2" using 1:3 with linespoints, "static_3" using 1:3 with linespoints

// eqvivalent dynamic damping Method 2
// time angleAvg momentum
// plot "dynamic_0" using 1:2 with linespoints, "dynamic_1" using 1:2 with linespoints, "dynamic_2" using 1:2 with linespoints, "dynamic_3" using 1:2 with linespoints
// plot "dynamic_0" using 1:3 with linespoints, "dynamic_1" using 1:3 with linespoints, "dynamic_2" using 1:3 with linespoints, "dynamic_3" using 1:3 with linespoints
// plot "dynamic_0" using 1:4 with linespoints, "dynamic_1" using 1:4 with linespoints, "dynamic_2" using 1:4 with linespoints, "dynamic_4" using 1:3 with linespoints
// plot "dynamic_0" using 2:3 with linespoints, "dynamic_1" using 2:3 with linespoints, "dynamic_2" using 2:3 with linespoints, "dynamic_3" using 2:3 with linespoints
// plot "dynamic_0" using 2:4 with linespoints, "dynamic_1" using 2:4 with linespoints, "dynamic_2" using 2:4 with linespoints, "dynamic_3" using 2:4 with linespoints

// compare
// plot "dynamic_0" using 2:4 with linespoints, "dynamic_1" using 2:4 with linespoints, "dynamic_2" using 2:4 with linespoints, "dynamic_3" using 2:4 with linespoints, "basic_0" using 2:3 with linespoints, "basic_1" using 2:3 with linespoints, "basic_2" using 2:3 with linespoints, "basic_3" using 2:3 with linespoints
// plot "dynamic_0" using 1:4 with linespoints, "dynamic_1" using 1:4 with linespoints, "dynamic_2" using 1:4 with linespoints, "dynamic_3" using 1:4 with linespoints, "basic_0" using 1:3 with linespoints, "basic_1" using 1:3 with linespoints, "basic_2" using 1:3 with linespoints, "basic_3" using 1:3 with linespoints
