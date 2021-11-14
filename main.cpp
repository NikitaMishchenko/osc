#include "oscillation_basic.h"

#include "fft/fftw_impl.h"

std::string DataPath = "C:/PostGrad/phd/2016-2021-M3/2016-2021-M3/";



namespace periods ///caclulate periods
{
    ///static bool decrease;

//    bool alterating(double data)
//    {
//        ///prev = false decrease
//        ///prev = true increase
//        if(decreas)
//        {
//            if(data < 0.0)
//                return false ///same signe
//
//            decrease = true;
//            return true; ///signe changed
//        }else{
//            if(data > 0.0)
//                return false
//            return true;
//        }
//
//    }


    int calculate_periods(oscillation D, std::string base_file_name)
    {std::cout << "calculate_periods\n";

        std::ofstream fout;

        int period_counter = 0;
        size_t i = 0;

        while(i < D.size())
        {
            if(D.dangle[i] <= 0)
            {
                fout.open(DataPath + base_file_name + std::to_string(period_counter));
                while( D.dangle[i] <= 0.0)
                {
                    fout << D.time[i] << "\t"
                            << D.angle[i] << "\t"
                                << D.dangle[i] << "\t"
                                    << D.ddangle[i] << "\n";
                    i++;
                    //std::cout << i << "\n";
                }
                fout.close();
            }
            period_counter++;

            std::cout << "period_counter = " << period_counter << std::endl;

            if(D.dangle[i]>=0.0)
            {
                fout.open(DataPath + base_file_name + std::to_string(period_counter));
                while( D.dangle[i] > 0.0)
                {
                    fout << D.time[i] << "\t"
                            << D.angle[i] << "\t"
                                << D.dangle[i] << "\t"
                                    << D.ddangle[i] << "\n";
                    i++;
                    //std::cout << i << "\n";
                }
                fout.close();
                period_counter++;
            }
            std::cout << "period_counter = " << period_counter << std::endl;
        }
        return period_counter;
    }

    namespace gnuplot
    {
        std::string file_name(std::string fn){
            return (std::string("\"") + fn + "\"");
        }
    }


    void PlotScript(std::string base_file_name, int period_counter)
    {
        std::ofstream fout(base_file_name);
        std::ofstream fout1;
        fout << "plot ";
            for(int i = 0; i < period_counter-1 ; i++)//period_counter;
            {
               fout << "\"" + std::to_string(i) + "\"" + " using 1:2 with lines notitle, ";
            }
        fout.close();

        fout.open(base_file_name + "_1");
        fout << "plot ";
            for(int i = 0; i < period_counter-1 ; i++)//period_counter;
            {
               fout << "\"" + std::to_string(i) + "\"" + " using 2:4 with lines notitle, \\" << std::endl;
            }
        fout.close();

        ///PNG PERIODS mz(a)
        fout.open(base_file_name + "_mz(a)");
        fout1.open(base_file_name + "_a(t)");

        ///*******
                fout << "set terminal png size 800,600;\n";
                fout << "set grid;\n";
                fout << "set xrange [-100:100]\n";
                fout << "set yrange [-400000:400000]\n";

                fout1 << "set terminal png size 800,600;\n";
                fout1 << "set grid;\n";

            for(int i = 0; i < period_counter-1 ; i++)//period_counter;
            {
                fout  << "set output \'mz(a)" + std::to_string(i/2) +".png\'" << std::endl;
                fout << std::string("plot ") +
                        gnuplot::file_name(std::to_string(i)) +  " using 2:4 with lines notitle lt rgb" + gnuplot::file_name("red")+ ", "
                        << gnuplot::file_name(std::to_string(i+1)) + " using 2:4 with lines notitle" + gnuplot::file_name("blue") << std::endl;

                fout1  << "set output \'a(t)" + std::to_string(i/2) +".png\'" << std::endl;
                fout1 << std::string("plot ") +
                        gnuplot::file_name(std::to_string(i)) +  " using 1:2 with lines notitle lt rgb" + gnuplot::file_name("red")+ ", "
                        << gnuplot::file_name(std::to_string(i+1)) + " using 1:2 with lines notitle" + gnuplot::file_name("blue") << std::endl;

                i++;
            }
        ///*********
        fout.close();
        fout1.close();


    }

    void SavePeriod(std::string file_name)
    {
        std::cout << "empty func\n";
    }

}



int main(int argc, char * argv[]){
//    AngleHistory A;
//        A.load_row("C:/PostGrad/phd/2016-2021-M3/2016-2021-M3/3942-ist.txt");
//        A.info();
//        //A.print();
        //oscillation B;
        ///oscillation A(DataPath + "3942-ist.txt");
        //A.print();
          //  A.info();
        //A.write(DataPath + "3942_o.txt");
        ///periods::PlotScript(DataPath + "Plotter", periods::calculate_periods(A,""));

        ///fftw::func();

        std::cout << "argc = " << argc << std::endl;

        std::cout << "argv = ";
            for (int i = 0; i != argc; i++)
                std::cout << argv[i] << std::endl;

        /// /home/mishnic/data_proc/wt_exp/transversive_rod/2016-2021-M3/3942-ist.txt
        oscillation A(argv[1]);
        std::vector<double> spectrum(A.size());

        osc::fftw::perfom_fftw(A, spectrum);

        for(auto k : spectrum)
            std::cout << k << std::endl;

        std::cout << "spectrum size = " << spectrum.size() << std::endl;

        std::ofstream output_file("spectrum");
            std::ostream_iterator<double> output_iterator(output_file, "\n");
        std::copy(spectrum.begin(), spectrum.end(), output_iterator);


//        std::vector<double> V;
//        std::cout << V.size() << std::endl;
//        V.push_back(0);
//        std::cout << V.size() << std::endl;
//        V.reserve(2);
//        std::cout << V.size() << std::endl;
//        V[1] = 1;
//        std::cout << V.size() << std::endl;
//        V.push_back(3);
//        std::cout << V.size() << std::endl;
//        std::cout << "V = " << V[0] << ", " << V[1] << ", " << V[2] << std::endl;

}
