#pragma once

#include <iostream>

#include <memory>
#include <vector>

#include "src/model/tr_rod_model_params.h"
#include "src/analize_coefficients/specific/pitch_momentum.h"
#include "src/flow/wt_flow.h"

class PitchStaticMomentum
{
public:
    PitchStaticMomentum(std::shared_ptr<PitchMomentum> pitchMomentum) : m_pitchMomentum(pitchMomentum)
    {}

    size_t size() const {return m_pitchStaticMomentum.size();}
    double at(const size_t index) const {return m_pitchStaticMomentum.at(index);}
    double& at(const size_t index) {return m_pitchStaticMomentum.at(index);}

    bool calculatePitchStaticMomentum()
    {
        if (m_pitchMomentum->size() <= 0)
            return false;

        const size_t size = m_pitchMomentum->size();

        m_pitchStaticMomentum.reserve(size);

        double value = 0;

        for (size_t index = 0; index < size; ++index)
        {
            if (m_dangle->at(index) <= 0 && m_dangle->at(index + 1) >= 0)
                value = m_pitchMomentum->at(index);

            m_pitchStaticMomentum.push_back(value);
        }

        m_pitchStaticMomentum.shrink_to_fit();

        return true;
    }

private:
    std::vector<double> m_pitchStaticMomentum;

    // input
    std::shared_ptr<std::vector<double>> m_dangle;
    std::shared_ptr<PitchMomentum> m_pitchMomentum;
};
