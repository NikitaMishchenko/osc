#pragma once

#include <math.h>



namespace wt_flow
{
    const double T0_KELVIN = 273.15;

    class Flow
    {
    private:
        double m_rho = 0;
        double m_velocity = 0;
        double m_T0 = 0;

        double m_dynamicPressure = 0;

        // reference
        double m_mach = 0;
        double m_reynolds = 0;


    public:
        ///CALCULATE FLOW
        void setToDefault()
        {
            m_velocity = 1;
            m_dynamicPressure = 1;
        }

        bool calculateDynamicPressure()
        {
            if (m_rho && m_velocity)
            {
                m_dynamicPressure = m_rho*m_velocity*m_velocity/2.0;
                return true;
            }
            else
            {
                return false;
            }
        }

        bool calculateFlow()
        {
            if (m_mach && m_T0)
            {
                m_velocity = 20.04*m_mach*sqrt((m_T0 + T0_KELVIN)/(1.0 + 0.2*m_mach*m_mach));

                if (!calculateDynamicPressure())
                    return true;
                else
                    return false;
            }
            else
            {
                return false;
            }
        }


        bool isCalculated()
        {
            return m_dynamicPressure && m_velocity;
        }


        double getDynamicPressure()
        {
            return m_dynamicPressure;
        }



        // todo remove
        double calculateVelocityFormula(const double& mach, const double& T0for )
        {
            return 20.04*mach*sqrt((T0for + T0_KELVIN)/(1.0 + 0.2*mach*mach));
        }


    };





}
