#pragma once

#include "oscillation/oscillation_basic.h"

class Periods
{
public:

private:

    std::vector<Oscillation> periods;
};

namespace periods ///caclulate periods
{

    inline std::vector<Oscillation> splitPeriods(const Oscillation& D)
    {
        std::cout << "split periods\n";

        std::vector<Oscillation> result;
        Oscillation buff;

        buff.reserve(D.size());
        /*buff.time.reserve(D.size());
        buff.angle.reserve(D.size());
        buff.dangle.reserve(D.size());
        buff.ddangle.reserve(D.size());*/

        size_t i = 0;
        int periodCounter = 0;

        while(i < D.size())
        {
            std::cout << periodCounter << std::endl;

            if(D.getDangle(i) <= 0)
            {
                while( D.getDangle(i) <= 0.0 && i < D.size())
                {
                    buff.push_back(D.getTime(i), D.getAngle(i), D.getDangle(i), D.getDdangle(i));
                    //std::cout << "buff time = " << buff.time[i] << " buff size = " << buff.size() << std::endl;

                    i++;
                }

                std::cout << "buff.size() = " << buff.size() << std::endl;
                result.push_back(buff);
                std::cout << "resultlist size (cout of the halfperiods) = " << result.size() << std::endl;
                std::cout << "last result size (data counts in period) = " << result[periodCounter].size() << std::endl;
                periodCounter++;

                buff.clear();
            }


            if(D.getDangle(i) > 0.0)
            {
                while( D.getDangle(i) >= 0.0 && i < D.size())
                {
                    buff.push_back(D.getTime(i), D.getAngle(i), D.getDangle(i), D.getDdangle(i));
                    //std::cout << "buff time = " << buff.time[i] << " buff size = " << buff.size() << std::endl;

                    i++;
                }

                std::cout << "buff.size() =" << buff.size() << std::endl;
                result.push_back(buff);
                std::cout << "resultlist size (cout of the halfperiods) = " << result.size() << std::endl;
                std::cout << "last result size (data counts in period) = " << result[periodCounter].size() << std::endl;
                periodCounter++;

                buff.clear();
            }


            std::cout << "****" << i << "****\n";
        }

        //for(size_t i = 0; i < result.at(0).time.size(); ++i)
        //    std::cout << "i = " << i << " " << result.at(0).time.at(i) << std::endl;

        //std::cout << "end of loop\n";
        return result;
    }


    /*
    * Разделение истории колебаний на периоды колебаний.
    */
    inline int calculate_periods(Oscillation D, std::string base_file_name)
    {std::cout << "calculate_periods\n";

        std::ofstream fout;

        int period_counter = 0;
        size_t i = 0;

        while(i < D.size())
        {
            if(D.getDangle(i) <= 0)
            {
                fout.open(base_file_name + std::to_string(period_counter));
                while( D.getDangle(i) <= 0.0)
                {
                    fout << D.getTime(i) << "\t"
                            << D.getAngle(i) << "\t"
                                << D.getDangle(i) << "\t"
                                    << D.getDdangle(i) << "\n";
                    i++;
                    //std::cout << i << "\n";
                }
                fout.close();
            }
            period_counter++;

            std::cout << "period_counter = " << period_counter << std::endl;

            if(D.getDangle(i)>=0.0)
            {
                fout.open(base_file_name + std::to_string(period_counter));
                while( D.getDangle(i) > 0.0)
                {
                    fout << D.getTime(i) << "\t"
                            << D.getAngle(i) << "\t"
                                << D.getDangle(i) << "\t"
                                    << D.getDdangle(i) << "\n";
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
        inline std::string file_name(std::string fn){
            return (std::string("\"") + fn + "\"");
        }
    }


    /**
    * Генерация скриптов отрисовки периодов колебаний
    */
    inline void PlotScript(std::string base_file_name, int period_counter)
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

    inline void SavePeriod(std::string file_name)
    {
        std::cout << "empty func\n";
    }

} // namespace periods
