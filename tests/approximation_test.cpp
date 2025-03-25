#include <gtest/gtest.h>

#include <string>

#include "oscillation/angle_history.h"
#include "gnusl_wrapper/approximation/nonlinear.h"

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
