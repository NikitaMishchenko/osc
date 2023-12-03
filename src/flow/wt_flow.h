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

    inline double calculateVelocityWT(const double machNumber, const double temperature0)
    {
        const double velocity = 20.04 * machNumber * sqrt((temperature0 + T0_KELVIN) / (1.0 + 0.2 * machNumber * machNumber));

        return velocity;
    }

    class Flow
    {
    public:
        Flow()
        {
            m_rho = 1;
            m_velocity = 1;
            m_T0 = 1;
            m_dynamicPressure = 1;
            m_mach = 1;
            m_reynolds = 1;
        }

        Flow(const std::string &fileName)
        {
            loadFile(fileName);
        }

        // from PTL data
        Flow(const double dynamicPressureKgForce, const double mach, const double temperature, const double reynolds)
        {
            m_mach = mach;

            m_reynolds = reynolds;

            m_T0 = temperature;

            m_velocity = calculateVelocityWT(mach, temperature);

            m_dynamicPressure = dynamicPressureKgForce * GRAVITATIONAL_ACCELERATION;

            m_rho = m_dynamicPressure / m_velocity / m_velocity * 2.0;
        }

        Flow(const double density,
             const double velocity,
             const double temperature,
             const double dynamicPressure,
             const double mach,
             const double reynolds) : m_rho(density),
                                      m_velocity(velocity),
                                      m_T0(temperature),
                                      m_dynamicPressure(dynamicPressure),
                                      m_mach(mach),
                                      m_reynolds(reynolds)
        {

            m_velocity = calculateVelocityWT(mach, temperature);

            // m_dynamicPressure = m_dynamicPressure * GRAVITATIONAL_ACCELERATION;

            m_rho = m_dynamicPressure / m_velocity / m_velocity * 2.0;
        }
    
        Flow(const Flow& flow)
        {
            m_rho = flow.m_rho;
            m_velocity = flow.m_velocity;
            m_T0 = flow.m_T0;
            m_dynamicPressure = flow.m_dynamicPressure;
            m_mach = flow.m_mach;
            m_reynolds = flow.m_reynolds;
        }

        /// CALCULATE FLOW
        void setToDefault()
        {
            m_velocity = 1;
            m_dynamicPressure = 1;
        }

        void setDynamicPressure(const double &dynamicPressure)
        {
            m_dynamicPressure = dynamicPressure;
        }

        void setReynolds(const double &reynolds)
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

        bool isCalculated() const
        {
            return m_dynamicPressure && m_velocity;
        }

        double getDynamicPressure() const
        {
            return m_dynamicPressure;
        }

        double getVelocity() const
        {
            return m_velocity;
        }

        double getDensity() const
        {
            return m_rho;
        }

        double getTemperature() const
        {
            return m_T0;
        }

        double getMach() const
        {
            return m_mach;
        }

        double getReynolds() const
        {
            return m_reynolds;
        }

        /**
         *   calculete @m_velocity from @m_T0, @m_mach
         */
        bool calculateVelocity()
        {
            if (m_T0 && m_mach)
            {
                m_velocity = calculateVelocityWT(m_T0, m_mach);
                return true;
            }

            return false;
        }

        bool calculateDensity()
        {
            if (m_velocity && m_dynamicPressure)
            {
                m_rho = m_dynamicPressure / m_velocity / m_velocity * 2.0;
                return true;
            }
            return false;
        }

        /// IO
        void print() const;
        std::string getInfoString() const;

        /**
         *   Load data from txt-file of avg flow data:
         *   Mach *machValue
         *   m_T0
         *   m_rho
         */
        bool loadFile(const std::string &fileName);
        bool saveFile(const std::string &fileNmae);

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
