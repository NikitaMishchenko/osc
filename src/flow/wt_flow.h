#pragma once

#include <math.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


namespace wt_flow
{
    const double T0_KELVIN = 273.15;
    const double GREVITATIONAL_ACCELERATION = 9.80665;

    class Flow
    {
    private:
        double m_rho = 0;
        double m_velocity = 0;
        double m_T0 = 0;

        double m_dynamicPressure = 0;

        // reference
        double m_mach = 0;
        double m_reynolds = 0; // normalized at 1 meter


    public:
        ///CALCULATE FLOW
        void setToDefault()
        {
            m_velocity = 1;
            m_dynamicPressure = 1;
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


        // todo remove
        double calculateVelocityFormula(const double& mach, const double& T0for )
        {
            return 20.04*mach*sqrt((T0for + T0_KELVIN)/(1.0 + 0.2*mach*mach));
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

        /// todo make outer function
        bool parsePTLfile(const std::string& fileName);
    };



} // namespace wt_flow
