#pragma once

#include <iostream>

#include <algorithm>
#include <memory>
#include <vector>
#include <tuple>
#include <cstdlib>
#include <sstream>

#include <boost/optional/optional.hpp>

#include "gnusl_proc/approximation/nonlinear.h"
#include "flow/wt_flow.h"
#include "analize_coefficients/specific/amplitude.h"

enum Method
{
    METHOD_1 = 1, // direct amplitude approximation
    METHOD_2      // combined
};

struct PitchMomentumBasic
{
    double momentum;

    double time;
    double timeI;
    double timeF;

    double angle;
    double angleI;
    double angleF;
};

const std::string FILE_NAME_DATA_TO_APPROX = "approx";

const std::string FILE_NAME_APPROX_RESULT = "approxRes";

class PitchDynamicMomentum
{
public:
    PitchDynamicMomentum(const std::shared_ptr<std::vector<double>> time,
                         const std::shared_ptr<std::vector<double>> angle,
                         const std::shared_ptr<std::vector<double>> dangle,
                         const std::shared_ptr<std::vector<double>> ddangle,
                         const std::shared_ptr<wt_flow::Flow> flow,
                         const std::shared_ptr<Model> model,
                         const int numberOfPeriods = 2,
                         const int mode = ABS_AMPLITUDE,
                         const int method = METHOD_1) : m_time(time),
                                                        m_angle(angle),
                                                        m_dangle(dangle),
                                                        m_ddangle(ddangle),
                                                        m_numberOfPeriods(numberOfPeriods),
                                                        m_flow(flow),
                                                        m_model(model),
                                                        m_mode(mode),
                                                        m_method(method),
                                                        m_angleAmplitude(m_time, m_angle, m_dangle)
    {
    }

    std::tuple<std::vector<PitchMomentumBasic>, std::vector<PitchMomentumBasic>, std::vector<PitchMomentumBasic>, std::vector<AngleAmplitudeBase>>
    getData() const
    {
        return std::make_tuple(m_pitchStaticMomentum, m_pitchMomentumBasicVector, m_pitchDynamicMomentum, m_angleAmplitude.m_angleAmplitudeBase);
    }

    std::string getPlottScript()
    {
        return m_plotApprox.str();
    }

    void setHiddenIndex(const std::string &specificName)
    {
        m_specificName = specificName;
    }

    bool calculateEqvivalentDampingCoefficients(int method)
    {
        m_method = method;

        bool isOk = false;
        std::vector<approximation::nonlinnear::ApproximationResult> eqvivalentDampingCoefficientVector;

        std::cout << "calculateEqvivalentDampingCoefficients\n";

        {
            // damping coeffiients assumed to be constant on each calcuilated point
            std::tie(isOk, eqvivalentDampingCoefficientVector) = calculateEqvivalentDampingCoefficient();

            // calcualating actual coefficients
            // m_dyn = -2I*n(Ampl)*V/q/s/l/l

            // -2.0*I*v/q/s/l/l // model flow
            const double coeff = -2.0 * m_model->getI() * m_flow->getVelocity() / m_flow->getDynamicPressure() / m_model->getS() / m_model->getL() / m_model->getL();

            PitchMomentumBasic pitchMomentumBasic;

            for (const auto &eqvivalentDampingCoefficient : eqvivalentDampingCoefficientVector)
            {
                pitchMomentumBasic.momentum = coeff * eqvivalentDampingCoefficient.lambda;

                pitchMomentumBasic.time = eqvivalentDampingCoefficient.arg;
                pitchMomentumBasic.timeI = eqvivalentDampingCoefficient.argInitial;
                pitchMomentumBasic.timeF = eqvivalentDampingCoefficient.argFinal;

                pitchMomentumBasic.angle = eqvivalentDampingCoefficient.func;
                pitchMomentumBasic.angleI = eqvivalentDampingCoefficient.funcInitial;
                pitchMomentumBasic.angleF = eqvivalentDampingCoefficient.funcFinal;

                m_pitchMomentumBasicVector.push_back(pitchMomentumBasic);
            }

            isOk = true;
        }

        if (isOk)
        {
            if (!calcuatePitchStaticMomentum())
                return false;

            if (m_dangle->empty() || m_pitchStaticMomentum.empty()) // || (m_dangle.size() != m_pitchStaticMomentum.size()))
                return false;

            // l/v model, flow
            const double coeff = m_model->getL() / m_flow->getVelocity();

            const size_t size = std::min(std::min(m_pitchMomentumBasicVector.size(), m_pitchStaticMomentum.size()),
                                         std::min(m_angle->size(), m_dangle->size()));

            PitchMomentumBasic dynamic;
            // based on momentumBasic
            int iBasic = 0, iStatic = 0;

            for (const auto &basicThrough : m_pitchMomentumBasicVector)
            {
                double staticMomentum, basicMomentum, angle, dangle;

                std::tie(staticMomentum, basicMomentum, angle, dangle) = getAtTimeForBasic(basicThrough.time,
                                                                                           m_pitchMomentumBasicVector,
                                                                                           m_pitchStaticMomentum,
                                                                                           m_time,
                                                                                           m_angle,
                                                                                           m_dangle);

                dynamic.momentum = staticMomentum * angle + coeff * basicMomentum * dangle;
                dynamic.time = basicThrough.time;

                m_pitchDynamicMomentum.push_back(dynamic);
            }

            isOk = true;
        }

        return isOk;
    }

    bool calcuatePitchStaticMomentum()
    {
        std::cout << "calcuatePitchStaticMomentum(): m_dangle.size() = " << m_dangle->size() << "\t"
                  << "m_ddangle.size() = " << m_ddangle->size() << "\n";

        const double M_fr = m_Mfr ? *m_Mfr : 0;                                                                     // fixme
        const double coeffFriction = M_fr / m_flow->calculateDynamicPressure() / m_model->getS() / m_model->getL(); // 1/q/s/l // model, flow
        const double Iz = m_model->getI();                                                                          // model

        if (m_dangle->empty() || m_ddangle->empty())
            return false;

        PitchMomentumBasic pitchMomentumBasic;
        double timeI = m_time->at(0);
        double timeF;

        for (size_t i = 0; i < m_dangle->size() - 1; i++)
        {
            if (m_dangle->at(i) <= 0 && m_dangle->at(i + 1) >= 0)
            {
                pitchMomentumBasic.momentum = Iz * m_ddangle->at(i) - coeffFriction;
                pitchMomentumBasic.time = m_time->at(i);

                m_pitchStaticMomentum.push_back(pitchMomentumBasic);
            }

            if (1 == m_pitchStaticMomentum.size())
                m_pitchStaticMomentum.at(0).timeI = m_time->at(0);
        }

        return true;
    }

private:
    std::tuple<double, double, double, double>
    getAtTimeForBasic(double targetTime,
                      const std::vector<PitchMomentumBasic> &pitcBasicMomentumVector,
                      const std::vector<PitchMomentumBasic> &pitchStaticMomentumVector,
                      const std::shared_ptr<std::vector<double>> timeVector,
                      const std::shared_ptr<std::vector<double>> angleVector,
                      const std::shared_ptr<std::vector<double>> dangleVector) const
    {
        double basicMomentum, staticMomentum, angle, dangle;

        {
            std::vector<PitchMomentumBasic>::const_iterator pitcBasicMomentumVectorIt = pitcBasicMomentumVector.begin();
            while (pitcBasicMomentumVectorIt != pitcBasicMomentumVector.end()-1 && 
                   pitcBasicMomentumVectorIt->time <= targetTime && 
                   (pitcBasicMomentumVectorIt->time+1) > targetTime)
            {
                pitcBasicMomentumVectorIt++;
            }
            basicMomentum = pitcBasicMomentumVectorIt->momentum;
        }

        {
            std::vector<PitchMomentumBasic>::const_iterator pitchStaticMomentumVectorIt = pitchStaticMomentumVector.begin();
            while (pitchStaticMomentumVectorIt != pitchStaticMomentumVector.end()-1 && 
                   pitchStaticMomentumVectorIt->time <= targetTime &&
                   (pitchStaticMomentumVectorIt+1)->time >= targetTime)
            {
                pitchStaticMomentumVectorIt++;
            }
            staticMomentum = pitchStaticMomentumVectorIt->momentum;
        }

        {
            std::vector<double>::const_iterator timeVectorIt = timeVector->begin();
            std::vector<double>::const_iterator angleVectorIt = angleVector->begin();
            std::vector<double>::const_iterator dangleVectorIt = dangleVector->begin();
            while (timeVectorIt != timeVector->end()-1 &&
                   angleVectorIt != angleVector->end()-1 &&
                   dangleVectorIt != dangleVector->end()-1 && 
                   *timeVectorIt <= targetTime &&
                   *(timeVectorIt+1) >= targetTime)
            {
                timeVectorIt++;
                angleVectorIt++;
                dangleVectorIt++;
            }
            angle = *angleVectorIt;
            dangle = *dangleVectorIt;
        }

        return std::make_tuple(staticMomentum, basicMomentum, angle, dangle);
    }

    void saveData(const std::string &fileName, std::vector<double> v1, std::vector<double> v2) const
    {
        std::ofstream fout(fileName);

        for (size_t i = 0; i < v1.size(); i++)
            fout << v1.at(i) << "\t" << v2.at(i) << "\n";
    }

    void saveData(const std::string &fileName, std::vector<double> v1, std::vector<double> v2, std::vector<double> v3) const
    {
        std::ofstream fout(fileName);

        for (size_t i = 0; i < v1.size(); i++)
            fout << v1.at(i) << "\t" << v2.at(i) << "\t" << v3.at(i) << "\n";
    }

    std::tuple<bool, std::vector<approximation::nonlinnear::ApproximationResult>>
    calculateEqvivalentDampingCoefficient()
    {
        std::vector<AngleAmplitudeBase>::const_iterator it = m_angleAmplitude.m_angleAmplitudeBase.begin();
        std::vector<double> dataToApproximateY;
        std::vector<double> dataToApproximateX;

        bool isOk = false;
        std::vector<approximation::nonlinnear::ApproximationResult> approximationResultVector;

        int periodCounter = 0;
        int indexS = 0;

        const std::string fileNameApproxRes = FILE_NAME_APPROX_RESULT + "_" + (m_specificName ? m_specificName.get() : "");
        std::ofstream foutApproxRes;
        if (m_specificName)
            foutApproxRes.open(fileNameApproxRes);

        while (m_angleAmplitude.m_angleAmplitudeBase.end() != it)
        {
            for (size_t i = 0; i < m_numberOfPeriods * 2; i++)
            {
                indexS = it - m_angleAmplitude.m_angleAmplitudeBase.begin();
                /*std::cout << "calculateEqvivalentDampingCoefficient: " << it->m_amplitudeIndexesFromInitialAngle
                          << " index = " << indexS << "\t"
                          << " size = " << m_angleAmplitude.m_angleAmplitudeBase.size() << "\n";*/
                dataToApproximateX.push_back(it->m_amplitudeTime);
                dataToApproximateY.push_back(it->m_amplitudeAngle);
                it++;

                if (m_angleAmplitude.m_angleAmplitudeBase.end() == it)
                    break;
            }

            if (dataToApproximateY.size() < 2)
                break;

            int errCode = approximation::codes::NEGATIVE;
            approximation::nonlinnear::ApproximationResult approximationResult;

            // Model Yi = A * exp(-lambda * t_i) + b
            std::tie(errCode, approximationResult) = approximation::nonlinnear::approximate(dataToApproximateX, dataToApproximateY);

            std::string specificName = std::string();
            if (m_specificName)
            {
                specificName = "_" + m_specificName.get() + "_";
                const std::string fileNameApprox = FILE_NAME_DATA_TO_APPROX + specificName + std::to_string(periodCounter);

                std::cout << "saving tmp file: \"" << fileNameApprox << "\"\n";

                saveData(fileNameApprox, dataToApproximateX, dataToApproximateY);

                foutApproxRes << approximationResult.A << "\t" << approximationResult.B << "\t" << approximationResult.lambda << "\n";
                std::cout << approximationResult.A << "\t" << approximationResult.B << "\t" << approximationResult.lambda << "\n";
            }

            dataToApproximateX.clear();
            dataToApproximateY.clear();

            if (approximation::codes::POSITIVE != errCode)
                continue;

            approximationResultVector.push_back(approximationResult);

            periodCounter++;
        }

        isOk = !approximationResultVector.empty();

        if (m_specificName)
        {
            std::cout << "saving plotApprox file\n";

            m_plotApprox << "filename(n) = sprintf(\"" << FILE_NAME_DATA_TO_APPROX << "_" << m_specificName.get() << std::string("_%d\", n)\n")
                         << "plot for [i=0:20] filename(i) using 1:2 with linespoints";
        }

        return std::make_tuple(isOk, approximationResultVector);
    }

    double getAvgAmplitude(const std::vector<double> &ampl, const int currentIndex, const int pointCounter) const
    {
        double avgAmpl = 0;

        for (int i = currentIndex; i < currentIndex + pointCounter; i++)
        {
            if (ampl.size() == currentIndex + pointCounter)
                break;

            avgAmpl += ampl.at(i);
        }

        return avgAmpl / pointCounter;
    }

    // INPUT
    std::shared_ptr<std::vector<double>> m_time;
    std::shared_ptr<std::vector<double>> m_angle;
    std::shared_ptr<std::vector<double>> m_dangle;
    std::shared_ptr<std::vector<double>> m_ddangle;
    std::shared_ptr<wt_flow::Flow> m_flow;
    std::shared_ptr<Model> m_model;
    std::shared_ptr<double> m_Mfr;

    // settings
    int m_numberOfPeriods; // количество периодов колебаний
    int m_mode;
    int m_method;
    boost::optional<std::string> m_specificName;

    // INTERNAL
    std::vector<PitchMomentumBasic> m_pitchStaticMomentum;
    std::vector<PitchMomentumBasic> m_pitchDynamicMomentum;
    std::vector<PitchMomentumBasic> m_pitchMomentumBasicVector;

    AngleAmplitude m_angleAmplitude;

    std::stringstream m_plotApprox;
};
