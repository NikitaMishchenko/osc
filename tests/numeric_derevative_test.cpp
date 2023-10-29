#include <gtest/gtest.h>

#include <cmath>
#include <vector>
#include <fstream>

#include <core/math.h>

TEST(TestDerevative, derevative1)
{
    std::vector<double> x, y, dy, ddy;

    const int dataSize = 1000;
    
    x.reserve(dataSize);
    y.reserve(dataSize);
    
    const double dx = 0.01;
    const double w = 2.5;

    for (int i = 0; i < dataSize; i++)
    {
        x.push_back(i*dx);
        y.push_back(sin(i*dx*w));
    }
    
    dy.reserve(dataSize);
    ddy.reserve(dataSize);


    // derevativeOrd1N2

    for (int i = 0; i < dataSize; i++)
    {
        dy.push_back(derevativeOrd1N2(y, dx, i));
        ddy.push_back(derevativeOrd2N2(y, dx, i));
    }

    std::ofstream foutY("/home/mishnic/data/data_proc/y_x");

    for (int i = 0; i < dataSize; i++)
    {
        foutY << x[i] << " " 
              << y[i] << " "
              << dy[i] << " "
              << ddy[i] << "\n";
    }

    std::cout << "dataSize = " << dataSize << "\n";

    // last 1 points not ok for dy
    // first and last 2 point not ok for ddy
    for (int i = 1; i < dataSize - 1; i++)
    {
        std::cout << i << " " << dy[i] << " " << w*cos(i*dx*w) << " " << std::abs(1 - std::abs(dy[i]/w/cos(i*dx*w))) << "\n";
        if (cos(i*dx*w) != 0)
            ASSERT_TRUE(std::abs(1 - std::abs(dy[i]/w/cos(i*dx*w))) < 0.001);

    }

    for (int i = 2; i < dataSize-1; i++)
    {
        std::cout << i << " " << ddy[i] << " " << -w*w*sin(i*dx*w) << " " << std::abs(1 - std::abs(ddy[i]/(-w*w*sin(i*dx*w)))) << "\n";

        if (sin(i*dx*w) != 0)
            ASSERT_TRUE(std::abs(1 - std::abs(ddy[i]/(-w*w*sin(i*dx*w)))) < 0.0001);
    }
}
