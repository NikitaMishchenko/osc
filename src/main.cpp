#include <string>
#include <utility>

#include "oscillation/oscillation_basic.h"
#include "fft/fftw_impl.h"
#include "periods/periods_base.h"
#include "oscillation/cut_oscillation_file.h"
#include "options.h"
#include "oscillation/wt_oscillation.h"
#include "flow/parse_ptl.h"

const double Pi = 3.14159265359;

namespace errCode
{
    enum codes
    {
        SUCCESS     = 0,
        FAIL        = 1,
        UNEXPECTED  = 2
    };
}

/*
* Perform FFT procedure via FFTW realization
*/
void performProcedureFft(const std::string& input_file_name, const std::string& output_file_name, double shiftAngle)
{
    std::cout << "perform_procedure_FFT(" << input_file_name << ", "
                                          << output_file_name << ", "
                                          << shiftAngle << "\n";

    std::cout << "init Oscillation\n";
    Oscillation A;//(input_file_name);

    if (!A.loadFile(input_file_name))
    {
        std::cout << "failed to Load Oscillation!\n";
        return;
    }

    A.info();

    // todo calculate angle
    A.move_angle(shiftAngle);


    osc::fftw::spectrum sp(A.size());

    std::cout << "Performing FFTW\n";
    osc::fftw::perfom_fftw(A, sp);


        std::cout << "sp size = " << sp.amplitude.size() << std::endl;


        std::ofstream fout(output_file_name);
            for(size_t i = 0; i < sp.amplitude.size(); i++ )
            {

                fout << i/Pi << "\t"
                        << sp.amplitude[i] << "\t"
                        << sp.real[i] << "\t"
                        << sp.img[i] << "\t\n";
            }
        fout.close();
}



/**
*    Data loaded as Oscillation object. So the the initial data is y and t. And while Oscillation constructed
*    y' and y'' is calculating.
*
*    Basically procedure finds periods of the signal and saves it in specific manner.
*    Each period splits into forward a backward movement
*    (todo) after that gnuplot script creating.
*/
void performProcedurePeriods(const std::string& fileName, const std::string& fileName2, const std::vector<double>& extraArguments)
{
    std::cout << "periods procedure performing...\n";

    std::ifstream fin();

    Oscillation D;
    D.loadFile(fileName);
    if(0 == D.size())
    {
       std::cerr << "Empty Oscillation data! Aborting procedure\n";
       return;
    }


    std::cout << "extra arguments size = " << extraArguments.size() << std::endl;

    int mergeHalfPeriods = 1;

    if (extraArguments.size() > 1)
    {
        mergeHalfPeriods = extraArguments.at(1);
        std::cout << "mergeHalfPeriods = " << mergeHalfPeriods <<  "\n";
    }


    std::vector<Oscillation> periodsList;

    periodsList = periods::splitPeriods(D);

    std::cout << "Data split into " << periodsList.size() << "periods\n";

    std::string outputFileName = std::string();

    std::ofstream fout;

    std::ofstream fout2(fileName + ".periods");

    for(size_t i = 0; i < periodsList.size();)
    {
        outputFileName = fileName2 + "_" + std::to_string(i);

        if (mergeHalfPeriods > 1)
        {
            outputFileName += "_" + std::to_string(i + mergeHalfPeriods);
        }

        outputFileName += "_pi";

        fout.open(outputFileName);

        for (int halfperiodsToFile = 0; (halfperiodsToFile < mergeHalfPeriods && i < periodsList.size()); halfperiodsToFile++)
        {
            fout << periodsList.at(i);

            if (i <  periodsList.size()-2)
            {
                fout2 << periodsList.at(i).time.at(0) << "\t"
                        << periodsList.at(i).angle.at(0) << "\t"
                        << periodsList.at(i).dangle.at(0) << "\t"
                        << periodsList.at(i).ddangle.at(0) << "\t"
                        << periodsList.at(i+2).time.at(0) - periodsList.at(i).time.at(0) << "\t"
                        << 1/(periodsList.at(i+2).time.at(0) - periodsList.at(i).time.at(0)) << "\n";
            }

            i++;
        }


        fout.close();
    }
    fout2.close();
}

int perform_procedure_cutFile(const std::string& initialFile, const double timeFrom, const double timeTo, const std::string& finalFile)
{
    if (!Oscillation_files::cut_file(initialFile, timeFrom , timeTo, finalFile))
    {
        std::cerr << "cut file failed!/n";
        return 1;
    }
    return 0;
}



void testFunc()
{
    Oscillation A("A");
    std::vector<Oscillation> arr_osc;
    arr_osc.push_back(A);
    arr_osc[0].write("A_osc");
}

/*
TODO
 фильтрация, опции, нормальный функционал, деление на логику и работу с файлами
*/


int main(int argc, char * argv[])
{
    Options opt;
    if (!opt.parse_options(argc, argv))
        std::cerr << "parse program_options: err occured\n";

    if (opt.exist("help"))
    {
        opt.show();
        opt.readme();

        return 0;
    }

    std::cout << "perfroming!\n";


    ///INIT PARAMS
    const std::string mode = opt.getMode();
    const std::string fileName = opt.getFileName();
    const std::string fileName2 = opt.getFileName2();
    const std::vector<double> extraArguments = opt.getArgs();


    int result = errCode::UNEXPECTED;


    if ("test" == mode)
    {
        std::cout << "performing All procedures\n";
        testFunc();
    }

    if ("FFT" == mode)
    {
        std::cout << "performing fft\n";
        performProcedureFft(fileName, fileName + "_spectrum", extraArguments[0]);
        result = errCode::SUCCESS;
    }

    if ("periods" == mode || "P" == mode)
    {
        std::cout << "performing Period\n";
        performProcedurePeriods(fileName, fileName2, extraArguments);
        result = errCode::SUCCESS;
    }

    if ("cut" == mode)
    {
        std::cout << "cut file performing\n";

        if(extraArguments.size() < 2)
            return 2;

        double timeFrom = extraArguments[0];
        double timeTo = extraArguments[1];


        std::string resultFileName = fileName2.empty() ? (fileName + "_t" + std::to_string(timeFrom) + "_" + std::to_string(timeTo)) : fileName2;
        perform_procedure_cutFile(fileName, timeFrom, timeTo,
                                  resultFileName);
        result = errCode::SUCCESS;
    }



    if ("WT" == mode)
    {
        std::cout << "wt test processing mode performing\n";

        /// FLOW->
        wt_flow::Flow flow;
        if (!flow.loadFile((fileName + "_flow")))
            return 1;

        flow.calculateFlow();
        flow.print();
        /// FLOW<-

        /// MODEL->
        Model model;
        if (!model.loadFile((fileName + "_model")))
            return 1;

        model.print();
        /// MODEL<-

        /// OSCILLATION->
        Oscillation oscillation;
        if (!oscillation.loadFile(fileName + "_osc"))
            return 1;
        /// OSCILLATION<-


        WtOscillation wtTest(oscillation, flow, model);

        wtTest.getMz();
        wtTest.saveMzData(fileName + "_mz");
        //todo check


        wtTest.getMzAmplitudeIndexes();
        wtTest.saveMzAmplitudeData(fileName + "_mz_amplitude");
        result = errCode::SUCCESS;
    }


    if ("flow" == mode)
    {
        wt_flow::Flow flow;
        if( wt_flow::parsePTLfile(fileName + ".ptl", flow, 5))
        {
            flow.saveFile(fileName + ".flow");
            std::cout << "File " << fileName << " parsed: success\n";
        }
        else
            std::cerr << "File " << fileName << " parsed: failed\n";
        result = errCode::SUCCESS;
    }


    if (result != errCode::SUCCESS)
    {
        std::cout << "procedure not performed successfully:\n";

        switch (result)
        {
            case errCode::UNEXPECTED:
                std::cerr << "\tUNEXPECTED errcode\n";
                break;
            case errCode::FAIL:
                std::cerr << "\tFAIL errcode\n";
                break;
            default:
                std::cerr << "err code missing\n";
                break;
        }
        return 1;
    }
    else
    {
        std::cout << "procedures performed! status: success\n";
        return 0;
    }

}

/**
*
* TODO
* Gtests, autotests
*
*/
