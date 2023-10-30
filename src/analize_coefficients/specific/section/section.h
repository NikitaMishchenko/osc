#pragma once

#include <iostream>

#include <tuple>
#include <exception>

#include "oscillation/wt_oscillation.h"
#include "oscillation/oscillation_basic.h"
#include "gnusl_wrapper/interpolation/interpolation.h"

class Section : public Oscillation
{
public:
    enum SectionType
    {
        ASCENDING,
        DESCENDING
    };

    bool calculateSection(const Oscillation &oscillation, const double targetAngle, const int sectionType, uint interpolationPoints = 10)
    {
        m_sectionType = sectionType;
        m_targetAngle = targetAngle;

        if (oscillation.size() == 0)
            return false;

        const uint minimalInterpolationPoints = 3;

        if (interpolationPoints < minimalInterpolationPoints)
            interpolationPoints = minimalInterpolationPoints;

        for (int i = minimalInterpolationPoints; i < oscillation.size() - minimalInterpolationPoints; i++)
        {

            std::function<bool(const Oscillation &, const int index, const double targetAngle)> comparator;

            if (ASCENDING == m_sectionType)
            {
                comparator = [](const Oscillation &oscillation, const int index, const double targetAngle)
                {
                    return (oscillation.getAngle(index) > targetAngle &&
                            targetAngle > oscillation.getAngle(index + 1));
                };
            }
            else if (DESCENDING == m_sectionType)
            {
                comparator = [](const Oscillation &oscillation, const int index, const double targetAngle)
                {
                    return (oscillation.getAngle(index) < targetAngle &&
                            targetAngle < oscillation.getAngle(index + 1));
                };
            }
            else
            {
                throw std::runtime_error("comparator for section not selected");
            }

            if (comparator(oscillation, i, targetAngle))
            {
                int indexFrom = i - interpolationPoints / 2;
                int indexTo = i + interpolationPoints / 2;

                if (indexFrom < 0)
                {
                    continue;
                }

                if (indexTo > oscillation.size())
                {
                    continue;
                }

                Function toSplineDangle =
                    sortFunction(Function(std::vector<double>(oscillation.angleBegin() + indexFrom,
                                                              oscillation.angleBegin() + indexTo),
                                          std::vector<double>(oscillation.dangleBegin() + indexFrom,
                                                              oscillation.dangleBegin() + indexTo)));

                GnuslSplineWrapper splineDangle(toSplineDangle);

                ///
                Function toSplineDdangle =
                    sortFunction(Function(std::vector<double>(oscillation.angleBegin() + indexFrom,
                                                              oscillation.angleBegin() + indexTo),
                                          std::vector<double>(oscillation.ddangleBegin() + indexFrom,
                                                              oscillation.ddangleBegin() + indexTo)));

                GnuslSplineWrapper splineDdangle(toSplineDdangle);

                this->push_back((oscillation.getTime(i) + oscillation.getTime(i + 1)) / 2, // bad one
                                targetAngle,
                                splineDangle.getEvaluation(targetAngle),
                                splineDdangle.getEvaluation(targetAngle));
            }
        }

        return true;
    }

    double getTargetAngle() const { return m_targetAngle; }
    int getSectionType() const { return m_sectionType; }

private:
    double m_targetAngle;
    int m_sectionType;
};

class Sections
{
public:
    Sections(const Oscillation &oscillation, const double sectionAngleStep) : m_oscillation(oscillation)
    {
        m_maxAngle = *std::max_element(oscillation.angleBegin(), oscillation.angleEnd());
        m_minAngle = *std::min_element(oscillation.angleBegin(), oscillation.angleEnd());
        m_sectionBorderValue = int(std::min(std::abs(m_maxAngle), std::abs(m_minAngle)) / sectionAngleStep) * sectionAngleStep;
        m_sectionAngleStep = sectionAngleStep;

        // descriptionStream << "Сечения в промежутке [" << -sectionBorderValue << ", " << sectionBorderValue << "]"
        //                   << std::endl;
    }

    void calculate()
    {
        for (int sectionAngle = -m_sectionBorderValue; sectionAngle <= m_sectionBorderValue;)
        {
            // descriptionStream << "Рассчет методом сечений для угла " << sectionAngle << " градусов\n";
            // std::cout << "sectionAngle = " << sectionAngle << "\n";

            bool isOk = false;

            {
                Section sectionAsc;
                sectionAsc.calculateSection(m_oscillation, sectionAngle, Section::ASCENDING);
                m_sectrionsVector.push_back(sectionAsc);
            }
            {
                Section sectionDesc;
                sectionDesc.calculateSection(m_oscillation, sectionAngle, Section::DESCENDING);
                m_sectrionsVector.push_back(sectionDesc);
            }

            sectionAngle += m_sectionAngleStep;
        }
    }

    std::tuple<bool, double, double, std::vector<Section>> getData() const
    {
        return std::make_tuple(true, m_minAngle, m_maxAngle, m_sectrionsVector);
    }

private:
    std::vector<Section> m_sectrionsVector;
    double m_maxAngle;
    double m_minAngle;
    double m_sectionBorderValue;
    double m_sectionAngleStep;

    Oscillation m_oscillation;
};

/*inline std::tuple<bool, double, double, std::vector<Section>>
calculateSectionVector(const Oscillation &oscillation, const double sectionAngleStep)
{

    const double maxAngle = *std::max_element(oscillation.angleBegin(), oscillation.angleEnd());
    const double minAngle = *std::min_element(oscillation.angleBegin(), oscillation.angleEnd());
    const int sectionBorderValue = int(std::min(std::abs(maxAngle), std::abs(minAngle)) / sectionAngleStep) * sectionAngleStep;

    // descriptionStream << "Сечения в промежутке [" << -sectionBorderValue << ", " << sectionBorderValue << "]"
    //                   << std::endl;
    std::vector<Section> sectionVector;

    for (int sectionAngle = -sectionBorderValue; sectionAngle <= sectionBorderValue;)
    {
        // descriptionStream << "Рассчет методом сечений для угла " << sectionAngle << " градусов\n";
        // std::cout << "sectionAngle = " << sectionAngle << "\n";

        bool isOk = false;

        {
            Section sectionAsc;
            sectionAsc.calculateSection(oscillation, sectionAngle, Section::ASCENDING);
            sectionVector.push_back(sectionAsc);
        }
        {
            Section sectionDesc;
            sectionDesc.calculateSection(oscillation, sectionAngle, Section::DESCENDING);
            sectionVector.push_back(sectionDesc);
        }

        sectionAngle += sectionAngleStep;
    }

    return std::make_tuple(true, minAngle, maxAngle, sectionVector);
}*/
