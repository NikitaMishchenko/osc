#include <gtest/gtest.h>

#include "oscillation/angle_history.h"
#include "oscillation/oscillation_basic.h"
#include "analize_coefficients/specific/pitch_dynamic_momentum.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"

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
    const size_t sizeOfTest = 3;

    for (size_t i = 0; i < sizeOfTest; i++)
    {
        b.time = i * step;

        base.push_back(b);
    }

    const double timeLastInRange = base.back().time;
    const double timeOutOfRangeUpper = timeLastInRange + step;
    ASSERT_EQ(timeLastInRange, helpers::getAtTime(base, timeOutOfRangeUpper).time);

    const double timeFirstInRange = base.front().time;
    const double timeOutOfRangeBelow = timeFirstInRange - step;
    ASSERT_EQ(timeFirstInRange, helpers::getAtTime(base, timeOutOfRangeBelow).time);

    ASSERT_TRUE(base.size() > 2);
    
    const double timeInRange = (base.begin() + 1)->time;
    ASSERT_EQ(timeInRange, helpers::getAtTime(base, timeInRange).time);

    double timeInRangeMoreThenOne = timeInRange + 0.1 * step;
    ASSERT_EQ(timeInRange, helpers::getAtTime(base, timeInRangeMoreThenOne).time);
}
