#pragma once

#include <vector>
#include <istream>

#include "operation_types.h"

class OperationPerformer
{
public:
    OperationPerformer()
    {
        m_type = Operations::UNHANDLED;
    }

    OperationPerformer(const std::vector<double> &data, Operations type) : m_data(data),
                                                                           m_type(type)
    {
    }

    virtual ~OperationPerformer()
    {
    }

    virtual Operations getOperationType() const
    {
        return m_type;
    }

    virtual void perform(){};
    virtual void load(std::istream &source){};

protected:
    std::vector<double> m_data;
    Operations m_type;
};

class MakeConstantSignal : public OperationPerformer
{
public:
    MakeConstantSignal(std::vector<double> data)
        : OperationPerformer(data, Operations::MAKE_CONSTANT_SIGNAL),
          m_prepData(data)
    {
        std::cout << "MakeConstantSignal ctr dataSize: " << data.size() << "\n";
    }

    virtual ~MakeConstantSignal()
    {
    }

    virtual void perform()
    {
        // performance using data interpretation
    }

private:
    // data interpretation
    DataForConstantSignal m_prepData; // todo move from origin place
};

class MultiplyHarmonic : public OperationPerformer
{
public:
    MultiplyHarmonic(std::vector<double> data)
        : OperationPerformer(data, Operations::MULTIPLY_HARMONIC),
          m_prepData(data)
    {
         std::cout << "MultiplyHarmonic ctr\n";
    }

    virtual ~MultiplyHarmonic() {}

    virtual void perform()
    {
        // performance using data interpretation
    }

private:
    DataForMultiplyAddHarmonic m_prepData;
};

class AddHarmonic : public OperationPerformer
{
public:
    AddHarmonic(std::vector<double> data)
        : OperationPerformer(data, Operations::ADD_MAKE_HARMONIC),
          m_prepData(data)
    {
        std::cout << "AddHarmonic ctr\n";
    }

    virtual ~AddHarmonic() {}

    virtual void perform()
    {
        // performance using data interpretation
    }

private:
    DataForMultiplyAddHarmonic m_prepData;
};

class MultiplySlopeLinnear : public OperationPerformer
{
public:
    MultiplySlopeLinnear(std::vector<double> data)
        : OperationPerformer(data, Operations::MULTIPLY_SLOPE_LINNEAR),
          m_prepData(data)
    {
        std::cout << "MultiplySlopeLinnear ctr\n";
    }

    virtual ~MultiplySlopeLinnear() {}

    virtual void perform()
    {
        // performance using data interpretation
    }

private:
    DataForSlopeLinnear m_prepData;
};

class AddSlopeLinnear : public OperationPerformer
{
public:
    AddSlopeLinnear(std::vector<double> data)
        : OperationPerformer(data, Operations::ADD_SLOPE_LINNEAR),
          m_prepData(data)
    {
        std::cout << "AddSlopeLinnear ctr\n";        
    }

    virtual ~AddSlopeLinnear() {}

    virtual void perform()
    {
        // performance using data interpretation
    }

private:
    DataForSlopeLinnear m_prepData;
};

class ScaleTime : public OperationPerformer
{
public:
    ScaleTime(std::vector<double> data)
        : OperationPerformer(data, Operations::SCALE_TIME),
          m_prepData(data)
    {
        std::cout << "ScaleTime ctr\n";
    }

    virtual ~ScaleTime() {}

    virtual void perform()
    {
        // performance using data interpretation
    }

private:
    DataForScale m_prepData;
};
        

class ScaleAngle : public OperationPerformer
{
public:
    ScaleAngle(std::vector<double> data)
        : OperationPerformer(data, Operations::SCALE_ANGLE),
          m_prepData(data)
    {
        std::cout << "ScaleAngle ctr\n";
    }

    virtual ~ScaleAngle() {}

    virtual void perform()
    {
        // performance using data interpretation
    }

private:
    DataForScale m_prepData;
};
