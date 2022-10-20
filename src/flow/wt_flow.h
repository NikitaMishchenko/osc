#pragma once

#include <math.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


namespace wt_flow
{
    const double T0_KELVIN = 273.15;
    const double GRAVITATIONAL_ACCELERATION = 9.80665;

    class Flow
    {
    public:
        Flow()
        {
            double m_rho = 1;
            double m_velocity = 1;
            double m_T0 = 1;
            double m_dynamicPressure = 1;
            double m_mach = 1;
            double m_reynolds = 1; 
        }

        Flow(const std::string& fileName)
        {
            loadFile(fileName);
        }

        ///CALCULATE FLOW
        void setToDefault()
        {
            m_velocity = 1;
            m_dynamicPressure = 1;
        }

        void setDynamicPressure(const double& dynamicPressure)
        {
            m_dynamicPressure = dynamicPressure;
        }

        void setReynolds(const double& reynolds)
        {
            m_reynolds = reynolds;
        }

        void setT0(double T0)
        {
            m_T0 = T0;
        }

        void setMach(double mach)
        {
            m_mach = mach;
        }

        /**
        *   @return true if ok
        */
        bool calculateDynamicPressure();
        bool calculateFlow();

        bool isCalculated()
        {
            return m_dynamicPressure && m_velocity;
        }

        double getDynamicPressure()
        {
            return m_dynamicPressure;
        }

        /**
        *   calculete @m_velocity from @m_T0, @m_mach
        */
        bool calculateVelocity()
        {
            if(m_T0 && m_mach)
            {
                m_velocity = 20.04*m_mach*sqrt((m_T0 + T0_KELVIN)/(1.0 + 0.2*m_mach*m_mach));
                return true;
            }

            return false;
        }

        bool calculateDensity()
        {
            if (m_velocity && m_dynamicPressure)
            {
                m_rho = m_dynamicPressure/m_velocity/m_velocity * 2.0;
                return true;
            }
            return false;
        }

        /// IO
        void print();

        /**
        *   Load data from txt-file of avg flow data:
        *   Mach *machValue
        *   m_T0
        *   m_rho
        */
        bool loadFile( const std::string& fileName);
        bool saveFile( const std::string& fileNmae);

    private:
        double m_rho = 0;
        double m_velocity = 0;
        double m_T0 = 0;

        double m_dynamicPressure = 0;

        // reference
        double m_mach = 0;
        double m_reynolds = 0; // normalized at 1 meter
    };

} // namespace wt_flow
