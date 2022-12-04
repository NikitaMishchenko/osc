#pragma once

#include "../core/function.h"
#include "../oscillation/wt_oscillation.h"
#include "../io_helpers/naive.h"

//namespace dynamic_coefficents
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
        DynamicPitchCoefficient(const WtOscillation &wtOscillation)
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
    
        std::tuple<bool, std::vector<double>, std::vector<double>, std::vector<double> >
        doCalculateEquation()
        {
            const double I = m_wtOscillation.getModel().getI();
            const double s = m_wtOscillation.getModel().getS();
            const double l = m_wtOscillation.getModel().getL();

            const double q = m_wtOscillation.getFlow().getDynamicPressure();
            const double v = m_wtOscillation.getFlow().getVelocity();

            const double w = m_wtOscillation.getW();

            const double constCoeff = v / q / s / l / l;

            std::vector<double> dynamicPart;
            std::vector<double> staticPart;
            std::vector<double> dynamicCoefficient;

            double mzStatic = sqrt(w/q/s/l*I);

            std::cout << "generalCoefficient = " << constCoeff << "\n";
            std::cout << "Iz = " << I << "\n";
            std::cout << "mzStatic = " << mzStatic << "\n";

            for (int i = 0; i < m_wtOscillation.size(); ++i)
            {
                dynamicPart.push_back(I*constCoeff*m_wtOscillation.getDdangle(i)/m_wtOscillation.getDangle(i));
                
                staticPart.push_back(-1.0*mzStatic*constCoeff*m_wtOscillation.getAngle(i)/m_wtOscillation.getDangle(i));

                dynamicCoefficient.push_back(dynamicPart.back() + staticPart.back());
            }

            return std::make_tuple(true, dynamicPart, staticPart, dynamicCoefficient);
        }

        bool doCalculateEnvelop()
        {
            return false;
        }

    private:
        WtOscillation m_wtOscillation;
        int m_method;
    };

//}
