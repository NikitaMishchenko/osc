#pragma once

#include <utility>

#include <vector>

#include <gsl/gsl_matrix.h>

#include "oscillation_basic.h"
#include "model/tr_rod_model_params.h"
#include "flow/wt_flow.h"
#include "filtration/gsl_filters.h"
#include "core/vector_helpers.h"

struct Mz
{
    double time;
    double mz;
};

class WtOscillation : public Oscillation
{
public:
    WtOscillation(){}

    WtOscillation(const AngleHistory &angleHistory) : Oscillation(angleHistory),
                                                      m_flow(wt_flow::Flow()),
                                                      m_model(Model())
    {
        calcAngleAmplitudeIndexes();
        calculateW();
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
    };

    virtual ~WtOscillation(){};

    // SPECIFIC METHODS
    // todo move to helpers
    /*void makeScaledDDAngle(const double &factor)
    {
        std::vector<double> scaledDDAngle;

        if (m_mz.mz.size() > 0)
        {
            scaledDDAngle = ddangle;

            if ((factor - 1.0) < 0.0000000001 &&
                (1.0 - factor) < 0.0000000001)
            {
                return scaledDDAngle;
            }

            for (size_t i = 0; i < scaledDDAngle.size(); i++)
                scaledDDAngle[i] *= factor;
        }

        return scaledDDAngle;
    }*/

    std::vector<double> getTimeAmplitude() const;
    std::vector<double> getAngleAmplitude() const;
    double getTimeAmplitude(const size_t index) const;
    double getAngleAmplitude(const size_t index) const;
    Model getModel() const;
    wt_flow::Flow getFlow() const {return m_flow;}
    double getW() const {return m_w;};
    std::vector<Mz> getMz(){return m_mz;}

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

    std::vector<Mz> m_mz;
    //std::vector<size_t> m_mzAmplitudeIndexes;
    std::vector<size_t> m_AngleAmplitudeIndexes;

    double m_w; // frequency of oscillation main mode

    wt_flow::Flow m_flow;

    Model m_model;
};
