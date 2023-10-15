#include <gtest/gtest.h>

#include <cmath>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>

// #include <gnuplot_wrapper.h>
//#include <gnuplot/gnuplot_wrapper.h>
//#include <gnuplot/data/data.h>
//#include <gnuplot/gnuplot_1d.h>

#include "oscillation/wt_oscillation.h"
#include "utils/function_generator/function_generator.h"
#include "analize_coefficients/specific/pitch_dynamic_momentum.h"
#include "analize_coefficients/specific/amplitude/utils.h"

TEST(TestOfTest, test1)
{
    ASSERT_TRUE(true);
}

/*TEST(TestGnuplot, BasicTest)
{
    {
        const size_t size = 100;

        gnuplot::Gnuplot1d gnuplot1d;
        std::vector<double> arg, func;

        gnuplot::png::PngProperties pngProperties(640, 480);

        gnuplot::png::PngFile pngFile("testPic");

        pngFile.setPngProperties(pngProperties);

        // gnuplot1d.setPngFile(pngFile);

        {
            arg.clear();
            func.clear();

            arg.reserve(size);
            func.reserve(size);

            for (size_t i = 0; i < size; i++) // todo wrap to esasy to use way
            {
                arg.push_back(i * 0.1);
                func.push_back(10 * arg.back());
            }

            gnuplot::DataFunction dataFunction(arg, func);

            dataFunction.setTitle("10*x");
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

            dataFunction.setTitle("x**2+x");
            dataFunction.setLineColor(gnuplot::BLACK);
            dataFunction.setLineType(gnuplot::LINES_POINTS);

            gnuplot1d.addDataToPlot(dataFunction);
        }

        {
            std::string data = "100*sin(x)";
            gnuplot::DataGnuplotFunction dataGnuplotFunction(data);

            // dataGnuplotFunction.setTitle("100*sin(x)");

            gnuplot1d.addDataToPlot(dataGnuplotFunction);
        }

        gnuplot1d.setPlotTitle("testTitle");
        gnuplot1d.act1dVector();
    }
}*/
/*
TEST(Test, Amplitude)
{
    std::vector<function_generator::FunctionProbeData> functionProbeDataVector;

    function_generator::FunctionGenerator functionGenerator;

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

            descriptionFunc << "f(arg) = " << coeff.at(1) << " * sin(" << coeff.at(0) << " * arg) "
                            << "/abs(arg)\n";

            functionProbeData.setData(function0,
                                      descriptionFunc.str(),
                                      time0,
                                      length,
                                      dt,
                                      coeff);
        }

        functionProbeDataVector.push_back(functionProbeData);
    }

    bool isOk = false;
    std::vector<Function> functionVector;

    std::tie(isOk, functionVector) = functionGenerator.actOnData(functionProbeDataVector);

    AngleHistory angleHistory(functionVector.at(0).getDomain(),
                              functionVector.at(0).getCodomain());

    Oscillation oscillation(angleHistory);

    std::vector<double> time = oscillation.getTime();
    // std::shared_ptr<std::vector<double>> time = std::make_shared<std::vector<double>>(oscillation.getTime());
    // std::shared_ptr<std::vector<double>> angle;
    // std::shared_ptr<std::vector<double>> dangle;

    amplitude::AngleAmplitude angleAmplitude(std::make_shared<std::vector<double>>(oscillation.getTime()),
                                             std::make_shared<std::vector<double>>(oscillation.getAngle()),
                                             std::make_shared<std::vector<double>>(oscillation.getDangle()));

    amplitude::AngleAmplitudeAnalyser angleAmplitudeAnalyser(angleAmplitude);

    angleAmplitudeAnalyser.getMostFrequentAmplitudeValue(10);

    std::vector<amplitude::AngleAmplitudeBase> sortedAmplitude = angleAmplitudeAnalyser.getSortedAmplitude();

    ///
    const size_t size = sortedAmplitude.size();
    std::vector<double> arg, func;

    for (const auto &k : sortedAmplitude)
        arg.push_back(k.m_amplitudeTime);

    for (const auto &k : sortedAmplitude)
        func.push_back(k.m_amplitudeAngle);

    // todo make wrap for easy to use
    {

        gnuplot::Gnuplot1d gnuplot1d;

        gnuplot::png::PngProperties pngProperties(640, 480);

        gnuplot::png::PngFile pngFile("testPic");

        pngFile.setPngProperties(pngProperties);

        // gnuplot1d.setPngFile(pngFile);

        gnuplot::DataFunction dataFunction(arg, func);

        dataFunction.setTitle("");
        dataFunction.setLineColor(gnuplot::RED);
        dataFunction.setLineType(gnuplot::LINES);

        gnuplot1d.addDataToPlot(dataFunction);

        gnuplot1d.setPlotTitle("");
        gnuplot1d.act1dVector();
    }
}*/



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
            std::vector<amplitude::AngleAmplitudeBase> amplitude;

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
