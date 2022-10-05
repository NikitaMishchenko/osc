#pragma once

#include <utility>

#include <vector>

#include "oscillation_basic.h"
#include "../model/tr_rod_model_params.h"
#include "../flow/wt_flow.h"

class WtOscillation : public Oscillation
{
public:

    WtOscillation(Oscillation oscillation, wt_flow::Flow flow, Model model) : Oscillation(oscillation),
                                                                              m_flow(flow),
                                                                              m_model(model),
                                                                              m_timeStamp( getTime(1) - getTime(0))
    {
    };

    virtual ~WtOscillation()
    {
    };

    virtual double getTimeStamp() const
    {
        return m_timeStamp;
    }

    virtual double getAngle(size_t index) const
    {
        return getAngle(index);
    }

    virtual const std::vector<double> getAngle() const
    {
        return getAngle();
    }


    // SPECIFIC METHODS
    bool getMz()
    {
        if (m_flow.isCalculated())
        {
            const double factor = m_model.getI()/m_flow.getDynamicPressure()/m_model.getS()/m_model.getL();

            std::cout << "\tI/(qsl) = " << factor << "\n";

            m_mz = makeScaledDDAngle(factor);

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
            if (getDangle(i-1) <= 0 && getDangle(i) > 0)
                m_mzAmplitudeIndexes.emplace_back(i);
        }

        return true;
    }

    // IO
    bool saveMzData(const std::string& fileName)
    {
        std::ofstream fout(fileName);

        this->info();

        if(!fout.is_open())
        {
            std::cerr << "saveMzData failed to create file " << fileName << "\n";
            return false;
        }


        for (size_t i = 0; i < this->size(); i++)
            fout << getTime(i) << "\t" << m_mz.at(i) << "\n";

        fout.close();

        return true;
    }

    bool saveMzAmplitudeData(const std::string& fileName)
    {
        std::ofstream fout(fileName);

        this->info();

        if(!fout.is_open())
        {
            std::cerr << "saveMzAmplitudeData failed to create file " << fileName << "\n";
            return false;
        }


        for (auto index : m_mzAmplitudeIndexes)
        {
            fout << getTime(index) << "\t" << m_mz.at(index) << "\n";
        }
        fout.close();

        return true;
    }

    private:
        //Oscillation m_oscillation;

        std::vector<double> m_mz;
        std::vector<size_t> m_mzAmplitudeIndexes;

        wt_flow::Flow m_flow;

        Model m_model;

        double m_timeStamp;
};
