#include <string>
#include <utility>

#include "oscillation/oscillation_basic.h"
#include "fft/fftw_impl.h"
#include "periods/periods_base.h"
#include "oscillation/cut_oscillation_file.h"
#include "options.h"
#include "oscillation/wt_oscillation.h"

const double Pi = 3.14159265359;

/*
* Perform FFT procedure via FFTW realization
*/
void perform_procedure_FFT(const std::string& input_file_name, const std::string& output_file_name, double shiftAngle)
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
void perform_procedure_Periods(const std::string& file_name)
{
    std::cout << "periods procedure performing...\n";

    Oscillation D(file_name);
        if(0 == D.size())
        {
            std::cerr << "Empty Oscillation data! Aborting procedure\n";
            return;
        }

    D.info();
    std::cout << "stg\n";

    std::vector<Oscillation> periodsList;
    periodsList = periods::splitPeriods(D);
    std::cout << periodsList.size() << std::endl;

    for(size_t i = 0; i < periodsList.size(); ++i)
    {
        std::cout << "i = " << i << std::endl;
        std::cout << periodsList.size() << std::endl;

        std::ofstream fout(std::to_string(i));
        fout << periodsList.at(i);


        fout.close();
    }
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
    const std::vector<double> extraArguments = opt.getArgs();


    if ("test" == mode)
    {
        std::cout << "performing All procedures\n";
        testFunc();
    }

    if ("FFT" == mode)
    {
        std::cout << "performing fft\n";
        perform_procedure_FFT(fileName, fileName + "_spectrum", extraArguments[0]);
    }

    if ("periods" == mode)
    {
        std::cout << "performing Period\n";
        perform_procedure_Periods(fileName);

    }

    if ("cut" == mode)
    {
        std::cout << "cut file performing\n";

        if(extraArguments.size() < 2)
            return 2;

        double timeFrom = extraArguments[0];
        double timeTo = extraArguments[1];

        perform_procedure_cutFile(fileName, timeFrom, timeTo,
                                  fileName + "_t" + std::to_string(timeFrom) + "_" + std::to_string(timeTo));
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


    }

    wt_flow::Flow flow;
    flow.parsePTLfile("test.ptl");

    std::cout << "procedures performed! status: success\n";
    return 0;
}

/**
*
* TODO
* Gtests, autotests
*
*/
