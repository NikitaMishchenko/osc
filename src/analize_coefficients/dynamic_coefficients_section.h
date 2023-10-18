#include <tuple>
#include <exception>

#include "oscillation/wt_oscillation.h"
#include "gnusl_wrapper/interpolation/interpolation.h"

enum SectionType
{
    ASCENDING,
    DESCENDING
};

std::tuple<bool, Oscillation>
makeSection(const Oscillation &oscillation, const double targetAngle, const int sectionType, uint interpolationPoints = 10)
{
    Oscillation section;

    if (oscillation.size() == 0)
        return std::make_tuple(false, oscillation);

    const uint minimalInterpolationPoints = 3;

    if (interpolationPoints < minimalInterpolationPoints)
        interpolationPoints = minimalInterpolationPoints;

    for (int i = minimalInterpolationPoints; i < oscillation.size() - minimalInterpolationPoints; i++)
    {

        std::function<bool(const Oscillation &, const int index, const double targetAngle)> comparator;

        if (ASCENDING == sectionType)
        {
            comparator = [](const Oscillation &oscillation, const int index, const double targetAngle)
            {
                return (oscillation.getAngle(index) > targetAngle &&
                        targetAngle > oscillation.getAngle(index + 1));
            };
        } 
        else if (DESCENDING == sectionType)
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
        //(oscillation.getAngle(i) > targetAngle &&
        // targetAngle > oscillation.getAngle(i + 1)))
        //(oscillation.getAngle(i) > targetAngle &&
        // targetAngle < oscillation.getAngle(i + 1))
        {

            Function toSplineDangle =
                sortFunction(Function(std::vector<double>(oscillation.angleBegin() + (i - interpolationPoints / 2),
                                                          oscillation.angleBegin() + (i + interpolationPoints / 2)),
                                      std::vector<double>(oscillation.dangleBegin() + (i - interpolationPoints / 2),
                                                          oscillation.dangleBegin() + (i + interpolationPoints / 2))));

            GnuslSplineWrapper splineDangle(toSplineDangle);

            ///
            Function toSplineDdangle =
                sortFunction(Function(std::vector<double>(oscillation.angleBegin() + (i - interpolationPoints / 2),
                                                          oscillation.angleBegin() + (i + interpolationPoints / 2)),
                                      std::vector<double>(oscillation.ddangleBegin() + (i - interpolationPoints / 2),
                                                          oscillation.ddangleBegin() + (i + interpolationPoints / 2))));

            GnuslSplineWrapper splineDdangle(toSplineDdangle);

            section.push_back((oscillation.getTime(i) + oscillation.getTime(i + 1)) / 2, // bad one
                              targetAngle,
                              splineDangle.getEvaluation(targetAngle),
                              splineDdangle.getEvaluation(targetAngle));
        }
    }

    return std::make_tuple(true, section);
}
