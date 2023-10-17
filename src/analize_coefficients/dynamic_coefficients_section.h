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
            if (targetAngle == -90)
            {
                std::ofstream fout("/home/mishnic/data/phd/sphere_cone_M1.75/4463/sectionInput");

                for (int j = i - interpolationPoints / 2; j < i + interpolationPoints / 2; j++)
                {
                    fout << *(oscillation.angleBegin() + j) << " "
                         << *(oscillation.dangleBegin() + j) << " "
                         << *(oscillation.ddangleBegin() + j) << "\n";
                }
            }

            Function toSplineDangle =
                                    sortFunction(Function
                                    (std::vector<double>(oscillation.angleBegin() + (i - interpolationPoints / 2),
                                                        oscillation.angleBegin() + (i + interpolationPoints / 2)),
                                    std::vector<double>(oscillation.dangleBegin() + (i - interpolationPoints / 2),
                                                        oscillation.dangleBegin() + (i + interpolationPoints / 2))));

            if (targetAngle == -90)
            {
                std::ofstream fout("/home/mishnic/data/phd/sphere_cone_M1.75/4463/sectionInput1");

                for (int j = i - interpolationPoints / 2; j < i + interpolationPoints / 2; j++)
                {
                    fout << *(oscillation.angleBegin() + j) << " "
                         << *(oscillation.dangleBegin() + j) << " "
                         << *(oscillation.ddangleBegin() + j) << "\n";
                }
            }

            GnuslSplineWrapper splineDangle(toSplineDangle);

            ///
            Function toSplineDdangle =
                                    sortFunction(Function
                                    (std::vector<double>(oscillation.angleBegin() + (i - interpolationPoints / 2),
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
