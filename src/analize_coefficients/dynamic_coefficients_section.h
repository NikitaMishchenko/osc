#include <tuple>

#include "oscillation/wt_oscillation.h"
#include "gnusl_wrapper/interpolation/interpolation.h"

std::tuple<bool, Oscillation>
makeSection(const Oscillation &oscillation, const double targetAngle, uint interpolationPoints = 10)
{
    Oscillation section;

    if (oscillation.size() == 0)
        return std::make_tuple(false, oscillation);

    const uint minimalInterpolationPints = 3;

    if (interpolationPoints < minimalInterpolationPints)
        interpolationPoints = minimalInterpolationPints;

    for (int i = minimalInterpolationPints; i < oscillation.size() - minimalInterpolationPints; i++)
    {
        if ((oscillation.getAngle(i) < targetAngle &&
            targetAngle < oscillation.getAngle(i + 1)))
        {

            ///

            GnuslSplineWrapper splineDangle(Function(std::vector<double>(oscillation.angleBegin() + (i - interpolationPoints / 2),
                                                                         oscillation.angleBegin() + (i + interpolationPoints / 2)),
                                                     std::vector<double>(oscillation.dangleBegin() + (i - interpolationPoints / 2),
                                                                         oscillation.dangleBegin() + (i + interpolationPoints / 2))));
            ///
            GnuslSplineWrapper splineDdangle(Function(std::vector<double>(oscillation.angleBegin() + (i - interpolationPoints / 2),
                                                                          oscillation.angleBegin() + (i + interpolationPoints / 2)),
                                                      std::vector<double>(oscillation.ddangleBegin() + (i - interpolationPoints / 2),
                                                                          oscillation.ddangleBegin() + (i + interpolationPoints / 2))));

            section.push_back((oscillation.getTime(i) + oscillation.getTime(i + 1)) / 2, // bad one
                              targetAngle,
                              splineDangle.getEvaluation(targetAngle),
                              splineDdangle.getEvaluation(targetAngle));
        }
    }

    return std::make_tuple(true, section);
}
