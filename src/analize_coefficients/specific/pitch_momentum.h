#pragma once

#include <iostream>

#include <memory>
#include <vector>

#include "src/model/tr_rod_model_params.h"
#include "src/flow/wt_flow.h"

class PitchMomentum
{
public:
    PitchMomentum(std::shared_ptr<std::vector<double>> dangle,
                  std::shared_ptr<std::vector<double>> ddangle,
                  std::shared_ptr<Model> model,
                  std::shared_ptr<wt_flow::Flow> flow) : m_dangle(dangle),
                                                         m_ddangle(ddangle),
                                                         m_model(model),
                                                         m_flow(flow)
    {
    }

    size_t size() const {return m_pitchMomentumValue.size();}
    double at(const size_t index) const {return m_pitchMomentumValue.at(index);}
    double& at(const size_t index) {return m_pitchMomentumValue.at(index);}

    bool calcultePitchMomentum()
    {
        const size_t size = m_dangle->size();

        if (size <= 1)
            return false;

        m_pitchMomentumValue.reserve(size);

        const double I = m_model->getI();
        const double s = m_model->getS();
        const double l = m_model->getL();
        const double q = m_flow->getDynamicPressure();
        const double frictionM = 0; // todo

        for (int i = 0; i < size; ++i)
        {
            m_pitchMomentumValue.push_back((I * m_ddangle->at(i) - frictionM) / q / s / l);
        }

        return true;
    }

private:
    std::vector<double> m_pitchMomentumValue;

    // input
    std::shared_ptr<std::vector<double>> m_dangle;
    std::shared_ptr<std::vector<double>> m_ddangle;
    std::shared_ptr<Model> m_model;
    std::shared_ptr<wt_flow::Flow> m_flow;
};
