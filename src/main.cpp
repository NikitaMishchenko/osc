#include <string>
#include <utility>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

#include "options.h"
#include "basic_procedures.h"

const double Pi = 3.14159265359;

int doJob(const options::Procedure procedureToPerform,
          const std::string& fileName,
          const std::string& fileName2,
          const std::vector<double>& extraArgumentsVector)
{
    int result = basic_procedures::UNEXPECTED;

    switch (procedureToPerform)
    {
    case options::CUT:
    {
        if (extraArgumentsVector.size() < 2)
        {
            std::cerr << "procedureCutFile err NOT enough extraArguments! aborting\n";
            result = basic_procedures::FAIL;
        }

        double timeFrom = extraArgumentsVector[0];
        double timeTo = extraArgumentsVector[1];

        std::string resultFileName = fileName2.empty() ? (fileName + "_t" + std::to_string(timeFrom) + "_" + std::to_string(timeTo)) : fileName2;

        result = basic_procedures::performProcedureCutFile(fileName, timeFrom, timeTo, resultFileName);

        break;
    }

    case options::FLOW:
    {
        result = basic_procedures::performProcedureFlow(fileName);
        break;
    }

    case options::PERIODS:
    {
        std::cout << "performing Period\n";

        result = basic_procedures::performProcedurePeriods(fileName, fileName2, extraArgumentsVector);

        break;
    }

    case options::WT:
    {
        result = basic_procedures::performProceduereProcessingWtData(fileName);

        break;
    }

    case options::PENDULUM:
    {
        if (extraArgumentsVector.size() < 2)
        {
            result = basic_procedures::performProcedurePendulum(fileName, 0, 0);
        }
        else
        {
            const uint32_t windowWidth = extraArgumentsVector.at(0);
            const uint32_t windowStep = extraArgumentsVector.at(1);

            result = basic_procedures::performProcedurePendulum(fileName, windowWidth, windowStep);
        }
        break;
    }

    case options::APPROXIMATION:
    {
        result = basic_procedures::performProcedureLinnearApproximation(fileName);
        break;
    }

    case ::options::COEFFICINETS_WINDOW:
    case ::options::DYN_COEFFICINETS_WINDOW_WT_TEST:
    {
        using namespace options;

        if (extraArgumentsVector.size() < 3) // or default arguments?
        {
            std::cerr << "Too few arguments int extraArgumentsVector! Aborting...\n";

            result = basic_procedures::FAIL;

            break;
        }

        const size_t indexFromData = extraArgumentsVector.at(0);
        const size_t indexToData = extraArgumentsVector.at(1);
        const size_t windowSize = extraArgumentsVector.at(2);
        const size_t stepSize = extraArgumentsVector.at(3);

        linnear_approximation::ApproxResultVector approxResultVector;

        if (COEFFICINETS_WINDOW == procedureToPerform)
        {
            std::tie(result, approxResultVector) = basic_procedures::performProcedurecCoefficientsFromWindow(fileName,
                                                                                                             indexFromData,
                                                                                                             indexToData,
                                                                                                             windowSize,
                                                                                                             stepSize);
        }
        else if (DYN_COEFFICINETS_WINDOW_WT_TEST == procedureToPerform)
        {
            boost::optional<double> moveAngeleAmplitudeValue = boost::none;

            if (extraArgumentsVector.size() < 4)
            {
                std::cerr << "Too few arguments int extraArgumentsVector! moveAngeleAmplitudeValue will unitilalised\n";
            }
            else
            {
                // to get negative value
                moveAngeleAmplitudeValue = (!extraArgumentsVector.at(4) ? -1.0*extraArgumentsVector.at(5) : extraArgumentsVector.at(4));
                std::cout << "Got moveAngleAmplitudeValue: " << moveAngeleAmplitudeValue.get() << "\n";
            }

            std::tie(result, approxResultVector) = basic_procedures::performProcedurecDynCoefficientsFromWindowForWtTest(fileName,
                                                                                                                         indexFromData,
                                                                                                                         indexToData,
                                                                                                                         windowSize,
                                                                                                                         stepSize,
                                                                                                                         moveAngeleAmplitudeValue);
        }

        
        const std::string suffix = fileName2;
        const std::string resultFileName = fileName + "_" + suffix + "_result";

        std::cout << "Saving ApproxResultVector to file: " << resultFileName << "\n";

        approxResultVector.save(resultFileName);

        break;
    }

    case options::FILTER_GAUSS:
    {
        std::cout << "prefroming FiterGauss\n";

        using namespace options;

        if (extraArgumentsVector.size() < 2)
        {
            std::cerr << "not ebouth extraArguments for gaauss filter\n";

            result = basic_procedures::FAIL;

            break;
        }

        const std::string fileNameInput = fileName;
        const std::string fileNameOutput = (!fileName2.empty() ? fileName2 : (std::string(fileName)+"_fGauss"));
        

        const size_t windowSize = extraArgumentsVector.at(0);
        const double alphaValue = extraArgumentsVector.at(1);

        AngleHistory angleHistory(fileName);

        result = basic_procedures::performProcedureFilterSignalViaGaussSimpleFitler(fileNameInput, fileNameOutput, windowSize, alphaValue);

        break;
    }


    case options::TEST:
    {
        std::cout << "performing All test procedures\n";

        result = basic_procedures::testFunc();

        break;
    }

    default:
    {
        result = basic_procedures::UNEXPECTED;

        result = basic_procedures::testFunc();

        break;
    }
    }

    std::cout << "procedures performed! status: ";

    switch (result)
    {
    case basic_procedures::SUCCESS:
        std::cout << "SUCESS\n";
        return 0;
    case basic_procedures::UNEXPECTED:
        std::cerr << "\tUNEXPECTED errcode\n";
        return result;
    case basic_procedures::FAIL:
        std::cerr << "\tFAIL errcode\n";
        return result;
    default:
        std::cerr << "err code missing\n";
        return 999;
    }

    return 666;
}

int main(int argc, char *argv[])
{
    std::cout << "argc = " << argc << "\n";
    /*if (1 == argc)
    {
        tests::makeAllTests();
        //doJob(options::TEST, std::string(), std::string(),std::vector<double>());
        return 0;
    }*/

    options::Options opt;

    try
    {
        if (!opt.parse_options(argc, argv))
            std::cerr << "parse program_options: err occurred\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "parse program_options: exception thrown: " << e.what() << "\n";
    }

    if (opt.exist("help"))
    {
        opt.show();
        opt.readme();

        return 0;
    }

    /// INIT PARAMS
    const options::Procedure procedureToPerform = opt.getProcedure();
    
    std::cout << "got procedure: " << static_cast<int>(procedureToPerform)
    << " which is " << options::helpers::toString(procedureToPerform)<< "\n"; // todo make string output
    
    const std::string fileName = opt.getFileName();
    const std::string fileName2 = opt.getFileName2();
    const std::vector<double> extraArgumentsVector = opt.getArgs();

    return doJob(procedureToPerform, fileName, fileName2, extraArgumentsVector);
}
