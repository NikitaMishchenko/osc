#include <gtest/gtest.h>

#include <vector>

#include "gnusl_wrapper/approximation/quadratic_approximation.h"

TEST(TestApproximationQuadratic, Approximation1)
{
    std::vector<double> x, y;

    double a = 2, b = 3, c = 0;
    x.push_back(-2);
    y.push_back(a * x[0] * x[0] + b * x[0] + c);

    x.push_back(-1);
    y.push_back(a * x[1] * x[1] + b * x[1] + c);

    x.push_back(0);
    y.push_back(a * x[2] * x[2] + b * x[2] + c);

    int errCode;
    std::vector<double> coefficients;

    std::tie(errCode, coefficients) = approxiamteQadratic(x, y);

    std::cout << "finished\n";

    std::cout << coefficients.at(2) << " " << a << "\n"
              << coefficients.at(1) << " " << b << "\n"
              << coefficients.at(0) << " " << c << "\n";

    ASSERT_TRUE(std::abs(a-coefficients.at(2)) < 0.0001);
    ASSERT_TRUE(std::abs(b-coefficients.at(1)) < 0.0001);
    ASSERT_TRUE(std::abs(c-coefficients.at(0)) < 0.0001);
}
