#pragma once

#include "oscillation_basic.h"
#include "tr_rod_model_params.h"
#include "wt_flow.h"

class WtOscillation
{

private:
    Oscillation m_oscillation;

    std::vector<double> m_mz;

    wt_flow::Flow m_flow;

    Model m_model;



public:

    WtOscillation(Oscillation oscillation, wt_flow::Flow flow, Model model) : m_oscillation(oscillation),
                                                                              m_flow(flow),
                                                                              m_model(model)
    {
    };


    bool getMz()
    {
        if (m_flow.isCalculated())
        {
            double factor = m_model.getI()/m_flow.getDynamicPressure()/m_model.getS()/m_model.getL();

            m_mz = m_oscillation.makeScaledDDAngle(factor);

            return true;
        }

        return false;
    }


    // IO
    bool saveMzData(const std::string& fileName)
    {
        std::ofstream fout(fileName);

        if(!fout.is_open())
        {
            for (size_t i = 0; i < m_oscillation.size(); i++)
                fout << m_oscillation.get_time(i) << "\t" << m_mz.at(i) << "\n";

            fout.close();

            return true;
        }
        else
        {
            return false;
        }
    }

};
