#include <utility>

#include <vector>

#include <gsl/gsl_matrix.h>

#include "oscillation_basic.h"
#include "src/model/tr_rod_model_params.h"
#include "src/flow/wt_flow.h"
#include "io_helpers/naive.h"
#include "filtration/gsl_filters.h"
#include "core/vector_helpers.h"


#include "wt_oscillation.h"

// SPECIFIC METHODS
bool WtOscillation::getMz() const
{
    if (m_flow.isCalculated())
    {
        const double factor = m_model.getI() / m_flow.getDynamicPressure() / m_model.getS() / m_model.getL();

        std::cout << "\tI/(qsl) = " << factor << "\n";

        // m_mz.mz = makeScaledDDAngle(factor);

        return true;
    }

    return false;
}

// todo move to helpers
/*void makeScaledDDAngle(const double &factor)
{
    std::vector<double> scaledDDAngle;

    if (m_mz.mz.size() > 0)
    {
        scaledDDAngle = ddangle;

        if ((factor - 1.0) < 0.0000000001 &&
            (1.0 - factor) < 0.0000000001)
        {
            return scaledDDAngle;
        }

        for (size_t i = 0; i < scaledDDAngle.size(); i++)
            scaledDDAngle[i] *= factor;
    }

    return scaledDDAngle;
}*/

std::vector<double> WtOscillation::getTimeAmplitude() const
{
    std::vector<double> result;

    result.reserve(m_AngleAmplitudeIndexes.size());

    for (size_t index = 0; index < m_AngleAmplitudeIndexes.size(); ++index)
        result.emplace_back(getTimeAmplitude(index));

    return result;
}

std::vector<double> WtOscillation::getAngleAmplitude() const
{
    std::vector<double> result;

    result.reserve(m_AngleAmplitudeIndexes.size());

    for (size_t index = 0; index < m_AngleAmplitudeIndexes.size(); ++index)
        result.emplace_back(getAngleAmplitude(index));

    return result;
}

double WtOscillation::getTimeAmplitude(const size_t index) const
{
    return m_domain.at(m_AngleAmplitudeIndexes.at(index));
}

double WtOscillation::getAngleAmplitude(const size_t index) const
{
    return m_codomain.at(m_AngleAmplitudeIndexes.at(index));
}

Model WtOscillation::getModel() const
{
    return m_model;
}

// IO
bool WtOscillation::saveMzData(const std::string &fileName) const
{
    if (m_mz.empty())
    {
        return false;
    }

    std::ofstream fout(fileName);

    this->info();

    if (!fout.is_open())
    {
        std::cerr << "saveMzData failed to create file " << fileName << "\n";
        return false;
    }

    for (size_t i = 0; i < m_mz.size(); i++)
    {
        fout << m_mz.at(i).time << "\t" << m_mz.at(i).mz << "\n";
        std::cout << getTime(i) << "\t" << m_mz.at(i).mz << "\n";
    }

    fout.close();

    return true;
}

bool WtOscillation::saveMzAmplitudeData(const std::string &fileName)
{
    std::ofstream fout(fileName);

    this->info();

    if (!fout.is_open())
    {
        std::cerr << "saveMzAmplitudeData failed to create file " << fileName << "\n";
        return false;
    }

    for (auto index : m_mzAmplitudeIndexes)
    {
        fout << getTime(index) << "\t" << m_mz.at(index).mz << "\n";
    }
    fout.close();

    return true;
}

// todo refactor: move to private
// first -time, second mz
bool WtOscillation::getMzAmplitudeIndexes()
{
    std::cout << "getMzAmplitudeIndexes entry()\n";
    std::cout << "\t\tm_mz size: " << m_mz.size() << "\n";

    if (m_mz.empty())
    {
        std::cerr << "WARNING m_mz is empty aborting\n";
        return false;
    }

    std::vector<double> tmpDangle = getDangle();

    tmpDangle = actLinnearGaussFilter(50,
                                      1,
                                      tmpDangle);

    for (size_t i = 1; i < m_mz.size() - 1; i++)
    {
        if (tmpDangle.at(i - 1) <= 0 && tmpDangle.at(i) > 0)
            m_mzAmplitudeIndexes.emplace_back(i);
    }

    return true;
}

bool WtOscillation::calcAngleAmplitudeIndexes()
{
    std::cout << "getMzAmplitudeIndexes entry()\n";

    if (AngleHistory::empty())
    {
        std::cerr << "WARNING AngleHistory is empty aborting\n";
        return false;
    }

    std::vector<double> tmpDangle = getDangle();

    std::ofstream fout1("ddangle_row");

    helpers::saveToFile(fout1, tmpDangle);

    fout1.close();

    tmpDangle = actLinnearGaussFilter(50,
                                      1,
                                      tmpDangle);

    std::ofstream fout2("ddangle_filt");

    helpers::saveToFile(fout2, tmpDangle);

    fout2.close();

    for (size_t i = 1; i < AngleHistory::size(); i++)
    {
        if (tmpDangle.at(i - 1) <= 0 && tmpDangle.at(i) > 0)
            m_AngleAmplitudeIndexes.emplace_back(i);

        if (tmpDangle.at(i - 1) >= 0 && tmpDangle.at(i) < 0)
            m_AngleAmplitudeIndexes.emplace_back(i);
    }

    m_AngleAmplitudeIndexes.shrink_to_fit();

    m_mz.reserve(m_AngleAmplitudeIndexes.size());

    Mz buff;

    for (size_t i = 0; i < m_AngleAmplitudeIndexes.size(); ++i)
    {
        buff.mz = m_codomain.at(m_AngleAmplitudeIndexes.at(i));
        buff.time = m_domain.at(m_AngleAmplitudeIndexes.at(i));

        m_mz.push_back(buff);
    }

    return true;
}

// othre
void WtOscillation::info() const
{
    std::cout << "WtOscillation oject\n"
              << "\tm_mz size: " << m_mz.size() << "\n"
              << "\tm_AngleAmplitudeIndexes size: " << m_AngleAmplitudeIndexes.size() << "\n"
              << "\tm_w = " << m_w << "\n"
              << "\tm_mzAmplitudeIndexes size: " << m_mzAmplitudeIndexes.size() << "\n";

    Oscillation::info();
}
