#include <sstream>

#include "wt_flow.h"

namespace wt_flow
{

    bool Flow::calculateDynamicPressure()
    {
        if (m_rho && m_velocity)
        {
            m_dynamicPressure = m_rho * m_velocity * m_velocity / 2.0;
            return true;
        }
        return false;
    }

    bool Flow::calculateFlow()
    {
        if (m_mach && m_T0)
        {
            m_velocity = 20.04 * m_mach * sqrt((m_T0 + T0_KELVIN) / (1.0 + 0.2 * m_mach * m_mach));

            return calculateDynamicPressure();
        }
        std::cerr << "Err wrong values: m_mach" << m_mach << " m_T0 = " << m_T0 << "\n";
        return false;
    }

    void Flow::print() const
    {
        std::cout << "Flow:\n"
                  << "\trho = " << m_rho << "\n"
                  << "\tvelocity = " << m_velocity << "\n"
                  << "\tT0 = " << m_T0 << "\n"
                  << "\tdynamicPressure  = " << m_dynamicPressure << "\n"
                  << "\tmach = " << m_mach << "\n"
                  << "\treynolds = " << m_reynolds << "\n";
    }

    std::string Flow::getInfoString() const
    {
        std::stringstream ss;

        ss << "Flow:\n"
           << "\trho = " << m_rho << "\n"
           << "\tvelocity = " << m_velocity << "\n"
           << "\tT0 = " << m_T0 << "\n"
           << "\tdynamicPressure  = " << m_dynamicPressure << "\n"
           << "\tmach = " << m_mach << "\n"
           << "\treynolds = " << m_reynolds << "\n";

        return ss.str();
    }

    bool Flow::loadFile(const std::string &fileName)
    {
        std::ifstream fin(fileName);

        if (!fin.is_open())
        {
            std::cerr << "flow: can't open file " << fileName << "\n";
            return false;
        }

        std::cout << "Parsing file " << fileName << "...\n";
        std::string buff_s;

        // mach = *mach* T0  = *T0* rho = *rho*
        fin >> buff_s >> buff_s >> m_mach >> buff_s >> buff_s >> m_reynolds >> buff_s >> buff_s >> m_T0 >> buff_s >> buff_s >> m_velocity >> buff_s >> buff_s >> m_dynamicPressure >> buff_s >> buff_s >> m_rho;

        fin.close();

        return true;
    }

    bool Flow::saveFile(const std::string &fileName)
    {
        std::ofstream fout(fileName);

        if (!fout.is_open())
        {
            std::cerr << "flow: can't open file " << fileName << "\n";
            return false;
        }

        // mach = *mach* T0  = *T0* rho = *rho*
        fout
            << "mach"
            << "\t=\t" << m_mach << "\n"
            << "Re"
            << "\t=\t" << m_reynolds << "\n"
            << "T0"
            << "\t=\t" << m_T0 << "\n"
            << "v"
            << "\t=\t" << m_velocity << "\n"
            << "q"
            << "\t=\t" << m_dynamicPressure << "\n";

        fout.close();

        return true;
    }

} // namespace wt_flow
