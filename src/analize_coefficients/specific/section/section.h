#pragma once

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
