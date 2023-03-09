#pragma once

#include <iostream>

#include <memory>
#include <vector>
#include <tuple>
#include <cstdlib>

#include "gnusl_proc/approximation/nonlinear.h"
#include "flow/wt_flow.h"
#include "analize_coefficients/specific/amplitude.h"

enum Mode
{
    ABS_AMPLITUDE
    // todo other mode
};

enum Method
{
    METHOD_1 = 1, // direct amplitude approximation
    METHOD_2      // combined
};

class PitchDynamicMomentum
{
public:
    PitchDynamicMomentum(std::shared_ptr<std::vector<double>> time,
                         std::shared_ptr<std::vector<double>> angle,
                         std::shared_ptr<std::vector<double>> dangle,
                         std::shared_ptr<std::vector<double>> ddangle,
                         std::shared_ptr<wt_flow::Flow> flow,
                         std::shared_ptr<Model> model,
                         const int numberOfPeriods = 2,
                         const int mode = ABS_AMPLITUDE,
                         const int method = METHOD_1) : m_time(time),
                                                        m_angle(dangle),
                                                        m_dangle(dangle),
                                                        m_ddangle(ddangle),
                                                        m_numberOfPeriods(numberOfPeriods),
                                                        m_flow(flow),
                                                        m_model(model),
                                                        m_mode(mode),
                                                        m_method(method),
                                                        m_angleAmplitude(time, angle, dangle)
    {
    }

    std::tuple<std::vector<double>, std::vector<double>>
    getData()
    {
        return std::make_tuple(m_pitchStaticMomentum, m_pitchMomentumBasic);
    }

    // todo unittests
    bool calculateEqvivalentDampingCoefficients(int method) 
    {
        m_method = method;

        if (!m_angleAmplitude.doWork())
            return false;

        bool isOk = false;
        std::vector<approximation::nonlinnear::ApproximationResult> eqvivalentDampingCoefficientVector;

        std::vector<double> dynamicDampingCoefficient;
        
        if (METHOD_1 == m_method)
        {
            // damping coeffiients assumed to be constant on each calcuilated point
            std::tie(isOk, eqvivalentDampingCoefficientVector) = calculateEqvivalentDampingCoefficient();

            // calcualating actual coefficients
            // m_dyn = -2I*n(Ampl)*V/q/s/l/l

            // -2.0*I*v/q/s/l/l // model flow
            const double coeff = -2.0*m_model->getI()*m_flow->getVelocity()/m_flow->getDynamicPressure()/m_model->getS()/m_model->getL()/m_model->getL();

            for (const auto &eqvivalentDampingCoefficient : eqvivalentDampingCoefficientVector)
            {
                dynamicDampingCoefficient.push_back(eqvivalentDampingCoefficient.lambda);
            }

            m_pitchMomentumBasic = dynamicDampingCoefficient;
            
            isOk = true;
        }
        else if (METHOD_2 == m_method)
        {
            if (!calcuatePitchStaticMomentum())
                return false;
            
            if (m_dangle->empty() || m_pitchStaticMomentum.empty()) // || (m_dangle.size() != m_pitchStaticMomentum.size()))
                return false;

            // l/v model, flow
            const double coeff = m_model->getL()/m_flow->getVelocity();

            for (size_t i = 0; i < m_pitchMomentumBasic.size(); ++i)
            {
               dynamicDampingCoefficient.push_back(m_pitchStaticMomentum.at(i) * m_angle->at(i) + coeff * m_pitchMomentumBasic.at(i) * m_dangle->at(i)); 
            }

            isOk = true;
        }

        return isOk;
    }

    bool calcuatePitchStaticMomentum()
    {
        std::cout << "calcuatePitchStaticMomentum(): m_dangle.size() = " << m_dangle->size() << "\t" << "m_ddangle.size() = " << m_ddangle->size() << "\n";

        const double M_fr = m_Mfr ? *m_Mfr : 0; // fixme 
        const double coeffFriction = M_fr/m_flow->calculateDynamicPressure()/m_model->getS()/m_model->getL(); // 1/q/s/l // model, flow
        const double Iz = m_model->getI(); // model

        if (m_dangle->empty() || m_ddangle->empty())
            return false;

        for (size_t i = 0; i < m_dangle->size()-1; i++)
        {
            if (m_dangle->at(i) <= 0 && m_dangle->at(i+1)<= 0)
                m_pitchStaticMomentum.push_back(Iz*m_ddangle->at(i) - coeffFriction);
        }

        return true;
    }

private:

    std::tuple<bool, std::vector<approximation::nonlinnear::ApproximationResult>>
    calculateEqvivalentDampingCoefficient()
    {
        std::vector<AngleAmplitudeBase>::const_iterator it = m_angleAmplitude.m_angleAmplitudeBase.begin();
        std::vector<double> dataToApproximateY;
        std::vector<double> dataToApproximateX;

        bool isOk = false;
        std::vector<approximation::nonlinnear::ApproximationResult> approximationResultVector;

        while (m_angleAmplitude.m_angleAmplitudeBase.end() != it)
        {
            for (size_t i = 0; i < m_numberOfPeriods * 2; i++, m_angleAmplitude.m_angleAmplitudeBase.end() != it)
            {
                dataToApproximateX.push_back(it->m_amplitudeTime);
                dataToApproximateY.push_back((ABS_AMPLITUDE == m_mode) ? std::abs(it->m_amplitudeAngle) : it->m_amplitudeAngle);
                it++;
            }

            if (dataToApproximateY.size() < 2)
                break;

            int errCode = approximation::codes::NEGATIVE;
            approximation::nonlinnear::ApproximationResult approximationResult;

            // Model Yi = A * exp(-lambda * t_i) + b
            std::tie(errCode, approximationResult) = approximation::nonlinnear::approximate(dataToApproximateX, dataToApproximateY);

            dataToApproximateX.clear();
            dataToApproximateY.clear();

            if (approximation::codes::POSITIVE != errCode)
                continue;

            approximationResultVector.push_back(approximationResult);
        }

        isOk = !approximationResultVector.empty();

        return std::make_tuple(isOk, approximationResultVector);
    }

    // INPUT
    std::shared_ptr<std::vector<double>> m_time;
    std::shared_ptr<std::vector<double>> m_angle;
    std::shared_ptr<std::vector<double>> m_dangle;
    std::shared_ptr<std::vector<double>> m_ddangle;
    std::shared_ptr<wt_flow::Flow> m_flow;
    std::shared_ptr<Model> m_model;
    std::shared_ptr<double> m_Mfr;

    // settings
    int m_numberOfPeriods; // количество периодов колебаний
    int m_mode;
    int m_method;

    // INTERNAL
    std::vector<double> m_pitchStaticMomentum;
    std::vector<double> m_pitchMomentumBasic;

    AngleAmplitude m_angleAmplitude;
};
