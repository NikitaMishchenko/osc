#pragma once

#include <string>
#include <vector>

enum Operations
{
    MAKE_CONSTANT_SIGNAL,
    MULTIPLY_HARMONIC,
    ADD_MAKE_HARMONIC,
    MULTIPLY_SLOPE_LINNEAR,
    ADD_SLOPE_LINNEAR,
    AMPLITUDE_DECREASE_LINNEAR,
    SCALE_TIME,
    SCALE_ANGLE,
    END,
    UNHANDLED
};

inline int getOperationDataPortionCount(Operations type)
{
    switch (type)
    {
        case (Operations::MAKE_CONSTANT_SIGNAL):
        case (Operations::SCALE_TIME):
        case (Operations::SCALE_ANGLE):
            return 3;

        case (Operations::ADD_SLOPE_LINNEAR):
        case (Operations::MULTIPLY_SLOPE_LINNEAR):
            return 4;

        case (Operations::MULTIPLY_HARMONIC):
        case (Operations::ADD_MAKE_HARMONIC):
            return 5;

        case (Operations::END):
        default:
            return 0;
    }
    return 0;
}

struct OperationContainer
{
    Operations type;
    std::vector<double> data;
};

struct DataForConstantSignal
{
    DataForConstantSignal(const std::vector<double> &data) : amplitude(data.at(0)),
                                                             size(int(data.at(1))),
                                                             timeStep(data.at(2))
    {
    }

    double amplitude;
    int size;
    double timeStep;
};

struct DataForMultiplyAddHarmonic
{
    DataForMultiplyAddHarmonic(const std::vector<double> &data) : indexFrom(size_t(data.at(0))),
                                                                  indexTo(size_t(data.at(1))),
                                                                  amplitude(data.at(2)),
                                                                  w(data.at(3)),
                                                                  phase(data.at(4))
    {
    }

    size_t indexFrom;
    size_t indexTo;
    double amplitude;
    double w;
    double phase;
};

struct DataForScale
{
    DataForScale(const std::vector<double> &data) : indexFrom(size_t(data.at(0))),
                                                    indexTo(size_t(data.at(1))),
                                                    scaleFactor(data.at(2))
    {
    }

    size_t indexFrom;
    size_t indexTo;
    double scaleFactor;
};

struct DataForSlopeLinnear
{
    DataForSlopeLinnear(const std::vector<double> &data) : indexFrom(size_t(data.at(0))),
                                                           indexTo(size_t(data.at(1))),
                                                           slopeA(data.at(2)),
                                                           slopeB(data.at(3))
    {
    }

    size_t indexFrom;
    size_t indexTo;
    double slopeA;
    double slopeB;
};