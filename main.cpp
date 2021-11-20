#include <string>

//#include <stdlib.h> //atof

#include "oscillation_basic.h"
#include "fft/fftw_impl.h"
#include "periods/periods_base.h"

#include "oscillation/cut_oscillation_file.h"




void perform_procedure_FFT(const std::string& file_name)
{

        oscillation A(file_name);

            ///calculate angle
            A.move_angle(-25.5);

        std::vector<double> spectrum(A.size());

        osc::fftw::perfom_fftw(A, spectrum);

        for(auto k : spectrum)
            std::cout << k << std::endl;

        std::cout << "spectrum size = " << spectrum.size() << std::endl;


        ///todo correct saving
        std::ofstream output_file("spectrum");
            std::ostream_iterator<double> output_iterator(output_file, "\n");
        std::copy(spectrum.begin(), spectrum.end(), output_iterator);

}

/**
    Data loaded as oscillation object. So the the initial data is y and t. And while oscillation constructed
    y' and y'' is calculating.

    Basically procedure finds periods of the signal and saves it in specific manner.
    Each period splits into forward a backward movement after that gnuplot script creating.
*/
void perform_procedure_Periods(const std::string& file_name)
{

}

void General_Procedure(int argc, char * argv[])
{

        std::cout << "argc = " << argc << std::endl;
        std::cout << "argv = ";
        for(int i = 0; i < argc; i++)
            std::cout << argv[i] << ", ";
        std::cout << std::endl << std::endl;

        if(argv[1])
        {
            if(std::string(argv[1]) == "fft")
            {

                std::cout << "FFT performing" << argv[2] << std::endl;

                if(argv[2])
                    perform_procedure_FFT(std::string(argv[2]));

            }


            if(std::string(argv[1]) == "period")
            {

                std::cout << "period performing" << argv[2] << std::endl;

                if(argv[2])
                    perform_procedure_Periods(std::string(argv[2]));

            }
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

int main(int argc, char * argv[])
{
    ///autotests()

    //General_Procedure(argc, argv);

    std::cout << "returned: " << Procedure_cut_file(argc, argv) << std::endl;

    ///tests
    ///perform test for each utill
}
