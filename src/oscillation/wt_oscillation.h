#pragma once

#include <utility>

#include <vector>

#include <gsl/gsl_matrix.h>

#include "oscillation_basic.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"
#include "gnusl_wrapper/filters/gauissian.h"
#include "core/vector_helpers.h"
#include "analize_coefficients/specific/amplitude/basic.h"

struct Mz
{
    double time;
    double mz;
};

inline double calcMzNondimensionalization(const wt_flow::Flow& flow, const Model& model)
{
    return (model.getI() / flow.getDynamicPressure() / model.getS() / model.getL());
}

inline double calcIzNondimentional(const wt_flow::Flow& flow, const Model& model)
{
    if (flow.getDensity() > 0)
        return 2 * model.getI() / flow.getDensity() / model.getS() / pow(model.getL(), 3);
    else
       return  0;
}

inline double calcWzNondimentional(const double wAut, const wt_flow::Flow& flow, const Model& model)
{
    return wAut * model.getL() / flow.getVelocity();
}

class WtOscillation : public Oscillation
{
public:
    WtOscillation() {}

    WtOscillation(const AngleHistory &angleHistory) : Oscillation(angleHistory),
                                                      m_flow(wt_flow::Flow()),
                                                      m_model(Model())
    {
        initialCalculation();
    };

    WtOscillation(const Oscillation &oscillation,
                  const wt_flow::Flow &flow,
                  const Model &model)
        : Oscillation(oscillation),
          m_flow(flow),
          m_model(model)
    {
        initialCalculation();
    };

    virtual ~WtOscillation(){};

    void initFromData(const Oscillation &oscillation,
                      const wt_flow::Flow &flow,
                      const Model &model)
    {
        *this = oscillation;
        m_flow = flow;
        m_model = model;

        initialCalculation();
    }

    double getMzNondimensionalization() const { return m_mzNondimensionalization; }
    double getIzNondimensional() const { return m_izNondimentional; }
    double getWzNondimentional() const { return m_wzNondimentional; }

    std::vector<double> getTimeAmplitude() const;
    std::vector<double> getAngleAmplitude() const;
    double getTimeAmplitude(const size_t index) const;
    double getAngleAmplitude(const size_t index) const;
    Model getModel() const;
    wt_flow::Flow getFlow() const { return m_flow; }
    double getW() const { return m_w; };

    amplitude::AngleAmplitudeVector getAngleAmplitudeVector() const
    {
        return m_angleAmplitudeVector;
    }

    // IO
    // todo refactor: move to private
    // first -time, second mz

    // othre
    virtual void info() const override;

private:
    void initialCalculation()
    {
        m_angleAmplitudeVector.initialize(std::make_shared<std::vector<double>>(this->getTime()),
                                          std::make_shared<std::vector<double>>(this->getAngle()),
                                          std::make_shared<std::vector<double>>(this->getDangle()));
        calculateW();
        m_mzNondimensionalization = calcMzNondimensionalization(m_flow, m_model);
        m_izNondimentional = calcIzNondimentional(m_flow, m_model);
        m_wzNondimentional = calcWzNondimentional(m_w, m_flow, m_model);
    }

    void calculateW()
    {
        /* freq at middle of data
        const size_t timeIndex1 = m_angleAmplitudeVector.at(m_angleAmplitudeVector.size()/2);
        const size_t timeIndex2 = m_angleAmplitudeVector.at(m_angleAmplitudeVector.size()/2 + 1);

        // 0.5 cos ampl indexies for top and bottom envelop
        m_w = 0.5 / (m_domain.at(timeIndex2) - m_domain.at(timeIndex1));
        */
        for (const auto &ampl : m_angleAmplitudeVector.m_angleAmplitudeData)
        {
            m_w += ampl.m_frequency;
        }

        m_w /= m_angleAmplitudeVector.m_angleAmplitudeData.size();

        std::cout << "w = " << m_w << "\n";
    }

    amplitude::AngleAmplitudeVector m_angleAmplitudeVector;

    double m_w; // frequency of oscillation main mode

    wt_flow::Flow m_flow;

    Model m_model;

    double m_mzNondimensionalization;
    double m_izNondimentional;
    double m_wzNondimentional;
};
