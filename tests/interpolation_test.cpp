#include <gtest/gtest.h>

#include <vector>
#include <cmath>

#include "gnusl_wrapper/interpolation/interpolation.h"

TEST(TestInterpolation, Interpolation1)
{
    std::vector<double> x;
    std::vector<double> y;

    double xBuff;
    for (int i = 0; i < 10; i++)
    {
        xBuff = i*0.1;
        x.push_back(xBuff);
        y.push_back(sin(xBuff));
    }

    Function dataToInterpolate(x, y);
    GnuslSplineWrapper spline(dataToInterpolate);

    // todo better value
    const double toleranceRef = 0.001;

    ASSERT_TRUE(std::abs(1 - spline.getEvaluation(0.005)/sin(0.005)) < toleranceRef);
    ASSERT_FALSE(std::abs(1 - spline.getEvaluation(0.9-0.023)/sin(0.9)) < toleranceRef);
}