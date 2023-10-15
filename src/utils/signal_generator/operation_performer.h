#pragma once

#include <vector>
#include <istream>

#include "operation_types.h"
#include "signal_generator_base.h"

// template<class Derived>
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

    virtual signal_generator::SignalGenerator *perform(signal_generator::SignalGenerator *signalGenerator)
    {
        return signalGenerator;
    };

protected:
    std::vector<double> m_data;
    Operations m_type;
};

class MakeConstantSignal : public OperationPerformer
{
public:
    MakeConstantSignal(const std::vector<double> &data)
        : OperationPerformer(data, Operations::MAKE_CONSTANT_SIGNAL),
          m_prepData(data)
    {
        std::cout << "MakeConstantSignal ctr dataSize: " << data.size() << "\n";
    }

    virtual ~MakeConstantSignal()
    {
    }

    virtual signal_generator::SignalGenerator *perform(signal_generator::SignalGenerator *signalGenerator)
    {
        // performance using data interpretation
        return signalGenerator->makeConstantSignal(m_prepData);
    }

private:
    // data interpretation
    DataForConstantSignal m_prepData; // todo move from origin place
};

class MultiplyHarmonic : public OperationPerformer
{
public:
    MultiplyHarmonic(const std::vector<double> &data)
        : OperationPerformer(data, Operations::MULTIPLY_HARMONIC),
          m_prepData(data)
    {
        std::cout << "MultiplyHarmonic ctr\n";
    }

    virtual ~MultiplyHarmonic() {}

    virtual signal_generator::SignalGenerator *perform(signal_generator::SignalGenerator *signalGenerator)
    {
        return signalGenerator->multiplyHarmonic(m_prepData);
    }

private:
    DataForMultiplyAddHarmonic m_prepData;
};

class AddHarmonic : public OperationPerformer
{
public:
    AddHarmonic(const std::vector<double> &data)
        : OperationPerformer(data, Operations::ADD_MAKE_HARMONIC),
          m_prepData(data)
    {
        std::cout << "AddHarmonic ctr\n";
    }

    virtual ~AddHarmonic() {}

    virtual signal_generator::SignalGenerator *perform(signal_generator::SignalGenerator *signalGenerator)
    {
        return signalGenerator->addHarmonic(m_prepData);
    }

private:
    DataForMultiplyAddHarmonic m_prepData;
};

class MultiplySlopeLinnear : public OperationPerformer
{
public:
    MultiplySlopeLinnear(const std::vector<double> &data)
        : OperationPerformer(data, Operations::MULTIPLY_SLOPE_LINNEAR),
          m_prepData(data)
    {
        std::cout << "MultiplySlopeLinnear ctr\n";
    }

    virtual ~MultiplySlopeLinnear() {}

    virtual signal_generator::SignalGenerator *perform(signal_generator::SignalGenerator *signalGenerator)
    {
        return signalGenerator->multiplySlopeLinnear(m_prepData);
    }

private:
    DataForSlopeLinnear m_prepData;
};

class AddSlopeLinnear : public OperationPerformer
{
public:
    AddSlopeLinnear(const std::vector<double> &data)
        : OperationPerformer(data, Operations::ADD_SLOPE_LINNEAR),
          m_prepData(data)
    {
        std::cout << "AddSlopeLinnear ctr\n";
    }

    virtual ~AddSlopeLinnear() {}

    virtual signal_generator::SignalGenerator *perform(signal_generator::SignalGenerator *signalGenerator)
    {
        return signalGenerator->addSlopeLinnear(m_prepData); // performance using data interpretation
    }

private:
    DataForSlopeLinnear m_prepData;
};

class AmplitudeDecreasLinnear : public OperationPerformer
{
public:
    AmplitudeDecreasLinnear(const std::vector<double> &data)
        : OperationPerformer(data, Operations::AMPLITUDE_DECREASE_LINNEAR),
          m_prepData(data)
    {
        std::cout << "AmplitudeDecreaseLinnear ctr\n";
    }

    virtual ~AmplitudeDecreasLinnear() {}

    virtual signal_generator::SignalGenerator *perform(signal_generator::SignalGenerator *signalGenerator)
    {
        return signalGenerator->amplitudeDecreaseLinnear(m_prepData); 
    }

private:
    DataForSlopeLinnear m_prepData;
};


class ScaleTime : public OperationPerformer
{
public:
    ScaleTime(const std::vector<double> &data)
        : OperationPerformer(data, Operations::SCALE_TIME),
          m_prepData(data)
    {
        std::cout << "ScaleTime ctr\n";
    }

    virtual ~ScaleTime() {}

    virtual signal_generator::SignalGenerator *perform(signal_generator::SignalGenerator *signalGenerator)
    {
        return signalGenerator->scaleTime(m_prepData);
    }

private:
    DataForScale m_prepData;
};

class ScaleAngle : public OperationPerformer
{
public:
    ScaleAngle(const std::vector<double> &data)
        : OperationPerformer(data, Operations::SCALE_ANGLE),
          m_prepData(data)
    {
        std::cout << "ScaleAngle ctr\n";
    }

    virtual ~ScaleAngle() {}

    virtual signal_generator::SignalGenerator *perform(signal_generator::SignalGenerator *signalGenerator)
    {
        return signalGenerator->scaleAngle(m_prepData);
    }

private:
    DataForScale m_prepData;
};

inline std::shared_ptr<OperationPerformer>
getOperationPerformer(const std::vector<double> &operationData,
                      Operations type)
{
    std::cout << "getOperationPerformer()";

    switch (type)
    {
    case (Operations::MAKE_CONSTANT_SIGNAL):
        return std::make_shared<MakeConstantSignal>(operationData);

    case (Operations::SCALE_TIME):
        return std::make_shared<ScaleTime>(operationData);

    case (Operations::SCALE_ANGLE):
        return std::make_shared<ScaleAngle>(operationData);

    case (Operations::MULTIPLY_HARMONIC):
        return std::make_shared<MultiplyHarmonic>(operationData);

    case (Operations::ADD_MAKE_HARMONIC):
        return std::make_shared<AddHarmonic>(operationData);

    case (Operations::ADD_SLOPE_LINNEAR):
        return std::make_shared<AddSlopeLinnear>(operationData);

    case (Operations::MULTIPLY_SLOPE_LINNEAR):
        return std::make_shared<MultiplySlopeLinnear>(operationData);
    
    case (Operations::AMPLITUDE_DECREASE_LINNEAR):
        return std::make_shared<AmplitudeDecreasLinnear>(operationData);

    case (Operations::END):
        return std::make_shared<OperationPerformer>();

    default:
        std::cerr << "smth wrong!";
    }

    return std::make_shared<OperationPerformer>();
}
