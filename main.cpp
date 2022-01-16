#include <string>


#include "oscillation_basic.h"
#include "fft/fftw_impl.h"
#include "periods/periods_base.h"

#include "oscillation/cut_oscillation_file.h"


#include "options.h"

const double Pi = 3.14;


void perform_procedure_FFT(const std::string& input_file_name, const std::string& output_file_name)
{

        oscillation A(input_file_name);

            ///calculate angle
            A.move_angle(-25.5); // fixme

        osc::fftw::spectrum sp(A.size());

        osc::fftw::perfom_fftw(A, sp);


        std::cout << "sp size = " << sp.amplitude.size() << std::endl;


        std::ofstream fout(output_file_name);
            for(size_t i = 0; i < sp.amplitude.size(); i++ )
            {

                fout << i/Pi << "\t"
                        << sp.amplitude[i] << "\t"
                        << sp.real[i] << "\t"
                        << sp.img[i] << "\t"
                        << std::endl;
            }
        fout.close();
}

/**
*    Data loaded as oscillation object. So the the initial data is y and t. And while oscillation constructed
*    y' and y'' is calculating.
*
*    Basically procedure finds periods of the signal and saves it in specific manner.
*    Each period splits into forward a backward movement
*    (todo) after that gnuplot script creating.
*/
void perform_procedure_Periods(const std::string& file_name)
{
    std::cout << "periods procedure performing...\n";

    oscillation D(file_name);
        if(0 == D.size())
        {
            std::cerr << "Empty oscillation data! Aborting procedure\n";
            return;
        }

    D.info();
    std::cout << "stg\n";

    std::vector<oscillation> periodsList;
    periodsList = periods::splitPeriods(D);
    std::cout << periodsList.size() << std::endl;

    for(size_t i = 0; i < periodsList.size(); ++i)
    {
        std::cout << "i = " << i << std::endl;
        std::cout << periodsList.size() << std::endl;

        std::ofstream fout(std::to_string(i));
        fout << periodsList.at(i);
        //periodsList.at(i).info();

        //std::cout << "size = " << periodsList[i].size() << std::endl;


        ///for(int j = 0; j < periodsList.at(i).angle.size(); j++)
           /// std::cout << periodsList.at(i).angle.at(j) << std::endl;

        fout.close();
    }
}


int Procedure_cut_file(int argc, char * argv[])
{
    std::cout << "argc = " << argc << std::endl;
    std::cout << "argv = ";
        for(int i = 0; i < argc; i++)
            std::cout << argv[i] << ", ";
        std::cout << std::endl << std::endl;


    std::ofstream log("cut.log", std::ios::app);
        log << "cut called from binary" << std::endl;
            log << "argc = " << argc << std::endl;
            log << "argv = \n";
                for(int i = 0; i < argc; i++)
                    log << argv[i] << " ";
                log << std::endl << std::endl;
    log.close();


    if (argv[1])
    if(std::string(argv[1]) == "cut")
    {
        if(argv[2] && argv[3] && argv[4] && argv[5])
        {
            std::string::size_type sz;

            std::string sarg1 = std::string(argv[3]);
            double arg1 = std::stod(sarg1, &sz);

            std::string sarg2 = std::string(argv[4]);
            double arg2 = std::stod(sarg2, &sz);

            std::cout << "cut_file performing ..." << argv[2] << argv[3] << argv[4] << argv[5] << std::endl;

            oscillation_files::cut_file(std::string(argv[2]), arg1 , arg2, std::string(argv[5]));

            return 0;
        }

        std::cout << "cut_file not performed! not enough arguments: " << argv[2] << argv[3] << argv[4] << argv[5] << std::endl;
        return 1;
    }

    if(std::string(argv[1]) == "cut_raw")
    {
        if(argv[2] && argv[3] && argv[4] && argv[5])
        {
            std::string::size_type sz;

            std::string sarg1 = std::string(argv[3]);
            double arg1 = std::stod(sarg1, &sz);

            std::string sarg2 = std::string(argv[4]);
            double arg2 = std::stod(sarg2, &sz);

            std::cout << "cut_file performing ..." << argv[2] << argv[3] << argv[4] << argv[5] << std::endl;

            oscillation_files::cut_raw_file(std::string(argv[2]), 5, arg1 , arg2, std::string(argv[5]));

            return 0;
        }

        std::cout << "cut_file not performed! not enough arguments: " << argv[2] << argv[3] << argv[4] << argv[5] << std::endl;
        return 1;
    }
    return 2;
}

void help() // todo
{
    std::cout << "set of utills for oscillation analysis" << std::endl;
        std::cout << "fft_osc" << std::endl; ///todo make object to push to stream;
        std::cout << "periods" << std::endl;
        std::cout << "cut file" << std::endl;
}


void General_Procedure(int argc, char * argv[])
{

        std::cout << "argc = " << argc << std::endl;
        std::cout << "argv = ";
        for(int i = 0; i < argc; i++)
            std::cout << argv[i] << ", ";
        std::cout << std::endl << std::endl;

        help();

        if(argv[1])
        {
            if(std::string(argv[1]) == "fft")
            {

                std::cout << "FFT performing" << argv[2] << std::endl;

                if(argv[2])
                    perform_procedure_FFT(std::string(argv[2]), std::string(argv[3]));

            }


            if(std::string(argv[1]) == "period")
            {

                std::cout << "period performing" << argv[2] << std::endl;

                if(argv[2])
                    perform_procedure_Periods(std::string(argv[2]));

            }


            if(std::string(argv[1]) == "cut" || std::string(argv[1]) == "cut_raw")
            {

                Procedure_cut_file(argc, argv);

            }

        }

}

void testFunc()
{
    oscillation A("A");
    std::vector<oscillation> arr_osc;
    arr_osc.push_back(A);
    arr_osc[0].write("A_osc");


}

/*
TODO
 фильтрация, опции, нормальный функционал, деление на логику и работу с файлами
*/


int main(int argc, char * argv[])
{

    ///perform_procedure_Periods("A");

    /*
    std::vector<oscillation> osc_v;
    oscillation ah;
        ah.push_back(1,1,1,1);
        ah.push_back(2,2,2,2);


    std::cout << ah << std::endl;

    for(int i = 0; i < 5; ++i)
    {
        std::cout << "push_back i = " << i << " size of ah = " << ah.size() << std::endl;
        osc_v.push_back(ah);
        std::cout << "size of last appended = " << osc_v[osc_v.size()-1].size() << std::endl;
        std::cout << "size of vector = " << osc_v.size() << std::endl;
    }

    std::cout << "vector of 5th ah:\n";
    for(auto k : osc_v)
    {
        std::cout << "size = " << k.size() << std::endl;
        std::cout << k << std::endl;
    }
    */


    Options opt;
    if(!opt.parse_options(argc, argv))
        std::cerr << "err occured\n";

    if(opt.exist("help"))
    {
        opt.show();
        return 0;
    }
    std::cout << "perfroming!\n";


    ///INIT PARAMS
    const std::string mode = opt.getMode();
    const std::string fileName = opt.getFileName();


    if("test" == mode)
    {
        std::cout << "performing All procedures\n";
        testFunc();
    }

    if("FFT" == mode)
    {
        std::cout << "performing fft\n";
        //perform_procedure_FFT();
    }

    if("periods" == mode)
    {
        std::cout << "performing Period\n";
        //perform_procedure_Periods(std::string(argv[2]));
        perform_procedure_Periods(fileName);

    }

    if("cut" == mode)
    {
        std::cout << "cut file performing\n";
        //Procedure_cut_file();
    }


    std::cout << "procedures performed! status: succes\n";
    return 0;
}

/**
* Gtests, autotests
*
*/
