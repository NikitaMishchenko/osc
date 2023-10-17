#pragma once

#include <utility>

#include <vector>

#include <gsl/gsl_matrix.h>

#include "oscillation_basic.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"
#include "gnusl_wrapper/filters/gauissian.h"
#include "core/vector_helpers.h"

struct Mz
{
    double time;
    double mz;
};

class WtOscillation : public Oscillation
{
public:
    WtOscillation() {}

    WtOscillation(const AngleHistory &angleHistory) : Oscillation(angleHistory),
                                                      m_flow(wt_flow::Flow()),
                                                      m_model(Model())
    {
        calcAngleAmplitudeIndexes();
        calculateW();
        calcMzNondimensionalization();
        calcIzNondimentional();
    };

    WtOscillation(const Oscillation &oscillation,
                  const wt_flow::Flow &flow,
                  const Model &model)
        : Oscillation(oscillation),
          m_flow(flow),
          m_model(model)
    {
        calcAngleAmplitudeIndexes();
        calculateW();
        calcMzNondimensionalization();
        calcIzNondimentional();
    };

    virtual ~WtOscillation(){};

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
    std::vector<Mz> getMz() { return m_mz; }

    // IO
    bool saveMzData(const std::string &fileName) const;

    bool saveMzAmplitudeData(const std::string &fileName);

    // todo refactor: move to private
    // first -time, second mz
    bool getMzAmplitudeIndexes();

    bool calcAngleAmplitudeIndexes();

    // othre
    virtual void info() const override;

private:
    void calculateW();
    void calcMzNondimensionalization()
    {
        m_mzNondimensionalization = m_model.getI() / m_flow.getDynamicPressure() / m_model.getS() / m_model.getL();
    }

    void calcIzNondimentional()
    {
        if (m_flow.getDensity() > 0)
            m_izNondimentional = 2 * m_model.getI() / m_flow.getDensity() / m_model.getS() / pow(m_model.getL(), 3);
        else
            m_izNondimentional = 0;
    }

    void calcWzNondimentional()
    {
        m_wzNondimentional = m_w * m_model.getL() / m_flow.getVelocity();
    }

    std::vector<Mz> m_mz;
    // std::vector<size_t> m_mzAmplitudeIndexes;
    std::vector<size_t> m_AngleAmplitudeIndexes;

    double m_w; // frequency of oscillation main mode

    wt_flow::Flow m_flow;

    Model m_model;

    double m_mzNondimensionalization;
    double m_izNondimentional;
    double m_wzNondimentional;
};
