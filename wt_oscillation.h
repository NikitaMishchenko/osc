#pragma once

#include <utility>

#include "oscillation_basic.h"
#include "tr_rod_model_params.h"
#include "wt_flow.h"

class WtOscillation
{

private:
    Oscillation m_oscillation;

    std::vector<double> m_mz;
    std::vector<size_t> m_mzAmplitudeIndexes;

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
            const double factor = m_model.getI()/m_flow.getDynamicPressure()/m_model.getS()/m_model.getL();

            std::cout << "\tI/(qsl) = " << factor << "\n";

            m_mz = m_oscillation.makeScaledDDAngle(factor);

            return true;
        }

        return false;
    }

    // todo
    // first -time, second mz
    bool getMzAmplitudeIndexes()
    {
        if (m_mz.empty())
            return false;

        for (size_t i = 1; i < m_mz.size()-1; i++)
        {
            if (m_oscillation.get_dangle(i-1) <= 0 &&  m_oscillation.get_dangle(i) > 0)
                m_mzAmplitudeIndexes.emplace_back(i);
        }

        return true;
    }

    // IO
    bool saveMzData(const std::string& fileName)
    {
        std::ofstream fout(fileName);

        m_oscillation.info();

        if(!fout.is_open())
        {
            std::cerr << "saveMzData failed to create file " << fileName << "\n";
            return false;
        }


        for (size_t i = 0; i < m_oscillation.size(); i++)
            fout << m_oscillation.get_time(i) << "\t" << m_mz.at(i) << "\n";

        fout.close();

        return true;
    }

    bool saveMzAmplitudeData(const std::string& fileName)
    {
        std::ofstream fout(fileName);

        m_oscillation.info();

        if(!fout.is_open())
        {
            std::cerr << "saveMzAmplitudeData failed to create file " << fileName << "\n";
            return false;
        }


        for (auto index : m_mzAmplitudeIndexes)
        {
            fout << m_oscillation.get_time(index) << "\t" << m_mz.at(index) << "\n";
        }
        fout.close();

        return true;
    }

};
