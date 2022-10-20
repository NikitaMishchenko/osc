#include <string>
#include <utility>

#include "options.h"
#include "basic_procedures.h"


const double Pi = 3.14159265359;

int doJob(const options::Procedure procedureToPerform,
          const std::string fileName,
          const std::string fileName2,
          const std::vector<double> extraArgumentsVector)
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

    case options::TEST:
    {
        std::cout << "performing All test procedures\n";

        result = basic_procedures::testFunc();

        break;
    }

    default:
    {
        result = basic_procedures::UNEXPECTED;

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
    std::cout << "got procedure: " << static_cast<int>(procedureToPerform) << "\n"; // todo make string output
    const std::string fileName = opt.getFileName();
    const std::string fileName2 = opt.getFileName2();
    const std::vector<double> extraArgumentsVector = opt.getArgs();

    return doJob(procedureToPerform, fileName, fileName2, extraArgumentsVector);
}
