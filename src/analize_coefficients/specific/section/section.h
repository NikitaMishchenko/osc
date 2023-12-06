#pragma once

#include <iostream>
#include <functional>
#include <tuple>
#include <exception>
#include <memory>

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

    bool calculateSection(std::shared_ptr<Oscillation> oscillationPtr, const double targetAngle, const int sectionType, uint interpolationPoints = 10)
    {
        m_sectionType = sectionType;
        m_targetAngle = targetAngle;

        if (oscillationPtr->size() == 0)
            return false;

        const uint minimalInterpolationPoints = 3;

        if (interpolationPoints < minimalInterpolationPoints)
            interpolationPoints = minimalInterpolationPoints;

        for (int i = minimalInterpolationPoints; i < oscillationPtr->size() - minimalInterpolationPoints; i++)
        {

            std::function<bool(std::shared_ptr<Oscillation> oscillationPtr, const int index, const double targetAngle)> comparator;

            if (ASCENDING == m_sectionType)
            {
                comparator = [](std::shared_ptr<Oscillation> oscillationPtr, const int index, const double targetAngle)
                {
                    return (oscillationPtr->getAngle(index) > targetAngle &&
                            targetAngle > oscillationPtr->getAngle(index + 1));
                };
            }
            else if (DESCENDING == m_sectionType)
            {
                comparator = [](std::shared_ptr<Oscillation> oscillationPtr, const int index, const double targetAngle)
                {
                    return (oscillationPtr->getAngle(index) < targetAngle &&
                            targetAngle < oscillationPtr->getAngle(index + 1));
                };
            }
            else
            {
                throw std::runtime_error("comparator for section not selected");
            }

            if (comparator(oscillationPtr, i, targetAngle))
            {
                int indexFrom = i - interpolationPoints / 2;
                int indexTo = i + interpolationPoints / 2;

                if (indexFrom < 0)
                {
                    continue;
                }

                if (indexTo > oscillationPtr->size())
                {
                    continue;
                }

                Function toSplineDangle =
                    sortFunction(Function(std::vector<double>(oscillationPtr->angleBegin() + indexFrom,
                                                              oscillationPtr->angleBegin() + indexTo),
                                          std::vector<double>(oscillationPtr->dangleBegin() + indexFrom,
                                                              oscillationPtr->dangleBegin() + indexTo)));

                GnuslSplineWrapper splineDangle(toSplineDangle);

                ///
                Function toSplineDdangle =
                    sortFunction(Function(std::vector<double>(oscillationPtr->angleBegin() + indexFrom,
                                                              oscillationPtr->angleBegin() + indexTo),
                                          std::vector<double>(oscillationPtr->ddangleBegin() + indexFrom,
                                                              oscillationPtr->ddangleBegin() + indexTo)));

                GnuslSplineWrapper splineDdangle(toSplineDdangle);

                this->push_back((oscillationPtr->getTime(i) + oscillationPtr->getTime(i + 1)) / 2, // bad one
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
    Sections()
    {}

    Sections(std::shared_ptr<Oscillation> oscillationPtr, const double sectionAngleStep) : m_oscillationPtr(oscillationPtr)
    {
        m_maxAngle = *std::max_element(oscillationPtr->angleBegin(), oscillationPtr->angleEnd());
        m_minAngle = *std::min_element(oscillationPtr->angleBegin(), oscillationPtr->angleEnd());
        m_sectionBorderValue = int(std::min(std::abs(m_maxAngle), std::abs(m_minAngle)) / sectionAngleStep) * sectionAngleStep;
        m_sectionAngleStep = sectionAngleStep;

        // descriptionStream << "Сечения в промежутке [" << -sectionBorderValue << ", " << sectionBorderValue << "]"
        //                   << std::endl;
    }

    void calculate()
    {
        double sectionAngle = -m_sectionBorderValue;

        while (sectionAngle <= m_sectionBorderValue)
        {
            {
                Section sectionAsc;
                sectionAsc.calculateSection(m_oscillationPtr, sectionAngle, Section::ASCENDING);
                m_sectrionsVector.push_back(sectionAsc);
            }

            {
                Section sectionDesc;
                sectionDesc.calculateSection(m_oscillationPtr, sectionAngle, Section::DESCENDING);
                m_sectrionsVector.push_back(sectionDesc);
            }

            sectionAngle += m_sectionAngleStep;
        }

        // for specific target angle get from ascending, get from descending ang calculate
        /*for (const auto& section : m_sectrionsVector)
        {
            Function funcDdangleOndangle;

            for (int i = 0; i < section.size()-1; i++)
            {
                double dangle = (section.getDangle(i+1) + section.getDangle(i)) / 2.0;
                double ddangleOndangle = (section.getDdangle(i+1) - section.getDdangle(i)) / (section.getDangle(i+1) - section.getDangle(i));
                
                funcDdangleOndangle.push_back(dangle, ddangleOndangle);
            }

            m_ddangleOndangleVector.push_back(funcDdangleOndangle);
        }*/
    }

    std::tuple<bool, double, double, std::vector<Section>, std::vector<Function>> getData() const
    {
        return std::make_tuple(true, m_minAngle, m_maxAngle, m_sectrionsVector, m_ddangleOndangleVector);
    }

    double sectionAngleStep() const
    {
        return m_sectionAngleStep;
    }

private:
    std::vector<Section> m_sectrionsVector;
    std::vector<Function> m_ddangleOndangleVector;
    double m_maxAngle;
    double m_minAngle;
    double m_sectionBorderValue;
    double m_sectionAngleStep;

    std::shared_ptr<Oscillation> m_oscillationPtr;
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
