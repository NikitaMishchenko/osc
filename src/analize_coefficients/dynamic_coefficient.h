#pragma once

#include <memory>

#include <string>
#include <sstream>

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
        std::cout << "DynamicPitchCoefficient ctr\n";
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

    std::vector<double> getAngle() const
    {
        return m_angle;
    }

    std::vector<double> getDangle() const
    {
        return m_dangle;
    }

    std::vector<double> getDdangle() const
    {
        return m_ddangle;
    }

    std::vector<double> getPitchMomentum() const
    {
        return m_pitchMomentum;
    }

    std::vector<double> getPitchStaticMomentum() const
    {
        return m_pitchStaticMomentum;
    }

    std::vector<double> getPitchDynamicMomentum() const
    {
        return m_pitchDynamicMomentum;
    }

    void calculate()
    {
        m_proceduresHistory << "prepareSignalData()\n";
        prepareSignalData();
        m_proceduresHistory << "calcultePitchMomentum()\n";
        calcultePitchMomentum();
        m_proceduresHistory << "calculatePitchStaticMomentum()\n";
        calculatePitchStaticMomentum();
        m_proceduresHistory << "calculatePitchDynamicMomentum()\n";
        calculatePitchDynamicMomentum();
    }

    /*std::tuple<bool, std::vector<double>, std::vector<double>, std::vector<double>>
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
    }*/

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
        std::cout << "m_pitchDynamicMomentum.size():" << m_pitchDynamicMomentum.size() << "\n";
    }

    std::tuple<std::vector<double>, std::vector<double>>
    pickPitchDynamicMomentumAtMaxAplitude() const
    {
        std::cout << "pickPitchDynamicMomentumAtMaxAplitude()\n";
        std::vector<double> result1, result2;

        if (m_pitchMomentum.empty() || m_dangle.empty())
            return std::make_tuple(result1, result2);

        result1.reserve(m_pitchMomentum.size());
        result2.reserve(m_pitchMomentum.size());

        const double l = m_wtOscillation->getModel().getL();
        const double v = m_wtOscillation->getFlow().getVelocity();

        double value = 0;

        if (m_pitchMomentum.size() != m_dangle.size())
            return std::make_tuple(result1, result2);

        for (size_t i = 0; i < m_pitchMomentum.size(); ++i)
        {
            if (isMaxAmplitude(i))
            {
                value = (m_pitchMomentum.at(i+1) - m_pitchMomentum.at(i)) * v / l * m_dangle.at(i);
            }

            if (value>0)
                result1.push_back(value);
            else
                result2.push_back(value);
        }

        result1.shrink_to_fit();
        result2.shrink_to_fit();

        return std::make_tuple(result1, result2);
    }

private:
    // between current and next index
    bool isMaxAmplitude(const size_t index) const
    {
        return (m_dangle.at(index) <= 0 && m_dangle.at(index + 1) >= 0);
    }

    bool prepareSignalData()
    {
        m_proceduresHistory << "actLinnearGaussFilter()\n";

        m_angle = actLinnearGaussFilter(5,
                                        1,
                                        m_wtOscillation->getAngle());

        m_dangle = actLinnearGaussFilter(5,
                                         1,
                                         m_wtOscillation->getDangle());

        m_ddangle = actLinnearGaussFilter(5,
                                          1,
                                          m_wtOscillation->getDdangle());

        return true;
    }

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
            m_pitchMomentum.push_back((I * m_ddangle.at(i) - frictionM) / q / s / l);
        }

        return true;
    }

    /*
     *   mz_static = mz(max(angle))
     */
    bool calculatePitchStaticMomentum()
    {
        m_pitchStaticMomentum.reserve(m_wtOscillation->size());

        const size_t sizeDangle = m_dangle.size();
        const size_t sizePitchMomentum = m_pitchMomentum.size();

        // from approximation
        // std::cout << "pitchStaticMomentum from w = " << m_wtOscillation.getW()*

        double value = 0;

        for (size_t i = 0; i < sizeDangle; ++i)
        {
            if (isMaxAmplitude(i))
                value = m_pitchMomentum.at(i);

            m_pitchStaticMomentum.push_back(value);
        }

        m_pitchStaticMomentum.shrink_to_fit();

        return true;
    }

    bool calculatePitchDynamicMomentum()
    {
        if (m_pitchStaticMomentum.empty())
            return false;

        const double l = m_wtOscillation->getModel().getL();
        const double v = m_wtOscillation->getFlow().getVelocity();

        m_pitchDynamicMomentum.reserve(m_pitchStaticMomentum.size());

        double fitValue = 0;

        std::cout << "m_pitchMomentum.size(): " << m_pitchMomentum.size() << "\n";
        std::cout << "m_pitchStaticMomentum.size(): " << m_pitchStaticMomentum.size() << "\n";

        if (m_pitchMomentum.size() != m_dangle.size())
            return false;

        m_pitchDynamicMomentum.resize(m_pitchMomentum.size());

        for (size_t i = 0; i < m_pitchMomentum.size(); ++i)
        {
            std::cout << "i = " << i << "\t"
                      << "m_pichMomentum: " << m_pitchMomentum.at(i) << "\n";
            //          << "m_pitchStaticMomentum*m_wtOscillation)" << m_pitchStaticMomentum.at(i) << "\n";

            // *m_wtOscillation->getAngle(i))
            // if (isMaxAmplitude(i))
            {
                fitValue = (m_pitchMomentum.at(i) - m_pitchMomentum.at(i)) * v / l * m_dangle.at(i);
            }

            m_pitchDynamicMomentum.push_back(fitValue);
        }

        return true;
    }

    std::shared_ptr<WtOscillation> m_wtOscillation;

    // for current processing may differ from m_wtOscillation
    std::vector<double> m_angle;
    std::vector<double> m_dangle;
    std::vector<double> m_ddangle;

    std::vector<double> m_pitchMomentum;
    std::vector<double> m_pitchStaticMomentum;
    std::vector<double> m_pitchDynamicMomentum;

    int m_method;

    std::stringstream m_proceduresHistory;
};

//}
