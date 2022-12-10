#pragma once

#include <memory>

#include "../core/function.h"
#include "../oscillation/wt_oscillation.h"
#include "../io_helpers/naive.h"

// namespace dynamic_coefficents
//{
enum CalculationMethod
{
    ENVELOP,
    EQUATION_FIRST_ORDER,
    NOT_SET
};

/**
 * m_domain time, m_codomain coefficient value
 * envelop
 */

class DynamicPitchCoefficient : public Function
{
public:
    DynamicPitchCoefficient(std::shared_ptr<WtOscillation> wtOscillation)
        : Function(), m_wtOscillation(wtOscillation), m_method(NOT_SET)
    {
    }

    ~DynamicPitchCoefficient() {}

    bool calculateCoefficient()
    {
        switch (m_method)
        {
        case (int(ENVELOP)):
        {
            return true;
        }

        case (int(EQUATION_FIRST_ORDER)):
        {
            return false; // doCalculateEquation();
        }

        case (int(NOT_SET)):
        {
            return false;
        }
        }

        return true;
    }

    std::vector<double> getPitchMomentum() const
    {
        return m_pitchMomentum;
    }

    std::vector<double> getPitchStaticMomentum() const
    {
        return m_pitchStaticMomentum;
    }

    void calculate()
    {
        calcultePitchMomentum();
        calculatePitchStaticMomentum();
    }

    std::tuple<bool, std::vector<double>, std::vector<double>, std::vector<double>>
    doCalculateEquation()
    {
        const double I = m_wtOscillation->getModel().getI();
        const double s = m_wtOscillation->getModel().getS();
        const double l = m_wtOscillation->getModel().getL();

        const double q = m_wtOscillation->getFlow().getDynamicPressure();
        const double v = m_wtOscillation->getFlow().getVelocity();

        const double w = m_wtOscillation->getW();

        const double constCoeff = v / q / s / l / l;

        std::vector<double> dynamicPart;
        std::vector<double> staticPart;
        std::vector<double> dynamicCoefficient;

        double mzStatic = sqrt(w / q / s / l * I);

        std::cout << "generalCoefficient = " << constCoeff << "\n";
        std::cout << "Iz = " << I << "\n";
        std::cout << "mzStatic = " << mzStatic << "\n";

        for (int i = 0; i < m_wtOscillation->size(); ++i)
        {
            dynamicPart.push_back(I * constCoeff * m_wtOscillation->getDdangle(i) / m_wtOscillation->getDangle(i));

            staticPart.push_back(-1.0 * mzStatic * constCoeff * m_wtOscillation->getAngle(i) / m_wtOscillation->getDangle(i));

            dynamicCoefficient.push_back(dynamicPart.back() + staticPart.back());
        }

        return std::make_tuple(true, dynamicPart, staticPart, dynamicCoefficient);
    }

    bool doCalculateEnvelop()
    {
        return false;
    }

    // todo 
    virtual void info()
    {
        m_wtOscillation->info();

        std::cout << "m_pitchMomentum.size():" << m_pitchMomentum.size() << "\n";
        std::cout << "m_pitchStaticMomentum.size():" << m_pitchStaticMomentum.size() << "\n";
    }

private:
    bool calcultePitchMomentum()
    {
        m_pitchMomentum.reserve(m_wtOscillation->size());

        // todo make it static internal ???
        const double I = m_wtOscillation->getModel().getI();
        const double s = m_wtOscillation->getModel().getS();
        const double l = m_wtOscillation->getModel().getL();

        const double q = m_wtOscillation->getFlow().getDynamicPressure();

        // todo
        const double frictionM = 0;

        for (int i = 0; i < m_wtOscillation->size(); ++i)
        {
            m_pitchMomentum.push_back((I * m_wtOscillation->getDdangle(i) - frictionM) / q / s / l);
        }

        return true;
    }

    bool calculatePitchStaticMomentum()
    {
        m_pitchStaticMomentum.reserve(m_wtOscillation->size());

        const size_t sizeDangle = m_wtOscillation->getDangle().size();
        const size_t sizePitchMomentum = m_pitchMomentum.size();

        double fitValue = 0.0; // todo move for half of period erlier

        // from approximation 
        // std::cout << "pitchStaticMomentum from w = " << m_wtOscillation.getW()*

        for (size_t i = 0; i < sizeDangle; ++i)
        {           
            if (m_wtOscillation->getDangle(i) <= 0 && m_wtOscillation->getDangle(i + 1) >= 0) // todo refactor
                fitValue = (m_pitchMomentum.at(i));

            m_pitchStaticMomentum.push_back(fitValue);
        }

        m_pitchStaticMomentum.shrink_to_fit();

        return true;
    }

    std::shared_ptr<WtOscillation> m_wtOscillation;

    std::vector<double> m_pitchMomentum;
    std::vector<double> m_pitchStaticMomentum;

    int m_method;
};

//}
