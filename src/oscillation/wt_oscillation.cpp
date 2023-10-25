#include <utility>

#include <vector>

#include <gsl/gsl_matrix.h>

#include "oscillation_basic.h"
#include "src/model/tr_rod_model_params.h"
#include "src/flow/wt_flow.h"
#include "io_helpers/naive.h"
#include "gnusl_wrapper/filters/gauissian.h"
#include "core/vector_helpers.h"

#include "wt_oscillation.h"

// SPECIFIC METHODS

std::vector<double> WtOscillation::getTimeAmplitude() const
{
    std::vector<double> result;

    result.reserve(m_angleAmplitudeVector.size());

    for (size_t index = 0; index < m_angleAmplitudeVector.size(); ++index)
        result.emplace_back(getTimeAmplitude(index));

    return result;
}

std::vector<double> WtOscillation::getAngleAmplitude() const
{
    std::vector<double> result;

    result.reserve(m_angleAmplitudeVector.size());

    for (size_t index = 0; index < m_angleAmplitudeVector.size(); ++index)
        result.emplace_back(getAngleAmplitude(index));

    return result;
}

double WtOscillation::getTimeAmplitude(const size_t index) const
{
    return m_domain.at(m_angleAmplitudeVector.at(index));
}

double WtOscillation::getAngleAmplitude(const size_t index) const
{
    return m_codomain.at(m_angleAmplitudeVector.at(index));
}

Model WtOscillation::getModel() const
{
    return m_model;
}

// IO

// todo refactor: move to private
// first -time, second mz
/*bool WtOscillation::getMzAmplitudeIndexes()
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
}*/

/*bool WtOscillation::calcAngleAmplitudeIndexes()
{
    if (AngleHistory::empty())
    {
        std::cerr << "WARNING AngleHistory is empty aborting\n";
        return false;
    }

    std::vector<double> tmpDangle = getDangle();

    tmpDangle = actLinnearGaussFilter(50,
                                      1,
                                      tmpDangle);

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

    const double factor = m_model.getI() / m_flow.getDynamicPressure() / m_model.getS() / m_model.getL();

    std::cout << "factor for mz = " << factor << "\n";

    for (size_t i = 0; i < m_AngleAmplitudeIndexes.size(); ++i)
    {
        buff.mz = m_codomain.at(m_AngleAmplitudeIndexes.at(i) * factor);
        buff.time = m_domain.at(m_AngleAmplitudeIndexes.at(i));

        m_mz.push_back(buff);
    }

    if (m_AngleAmplitudeIndexes.empty())
        return false;

    return true;
}*/

// othre
void WtOscillation::info() const
{
    std::cout << "WtOscillation oject\n"
              << "\tm_mz size: " << m_mz.size() << "\n"
              << "\tm_angleAmplitudeVector size: " << m_angleAmplitudeVector.size() << "\n"
              << "\tm_w = " << m_w << "\n";

    Oscillation::info();
}
