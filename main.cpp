#include <iostream>

#include <fstream>

#include <vector>
//#include <deque>
#include <string>

#include <iterator>
#include <algorithm>


const std::string DataPath = "C:/PostGrad/phd/2016-2021-M3/2016-2021-M3/";


enum LOG_MODE{
    LOG_ON,
    LOG_OFF
};

class AngleHistory{
public:
    std::vector<double> angle;
    std::vector<double> time;

    AngleHistory(){
        std::cerr << "default AngleHistory()\n";
    }

    virtual void load_row(std::string file_name)
    {
        std::ifstream fin(file_name);
        while(!fin.eof())
        {
            double b_angle, b_time;
                fin >> b_time >> b_angle;
                    angle.push_back(b_angle);
                    time.push_back(b_time);
//                std::cerr
//                        << "angle = " << angle.back()
//                        << "angle = " << angle.back()

        }
        fin.close();
    }

    AngleHistory(std::string file_name){
        std::cerr << "AngleHistory(file_name) constructor\n";
        this->load_row(file_name);
            this->info();
    }

    ~AngleHistory(){
        angle.clear();
        time.clear();
    }

    virtual void print(){
        for(size_t i = 0; i < angle.size(); i++)
        {
            std::cout << time[i] << "\t" << angle[i] << std::endl;
        }
    }

    virtual size_t size(){ return angle.size();}

    virtual void info()
    {
        std::cout << "AngleHistory object \nsize = ";
        std::cout << angle.size() << std::endl;
    }
};

class oscillation : public AngleHistory
{
public:
    std::vector<double> dangle;
    std::vector<double> ddangle;

    oscillation(): AngleHistory(), dangle()
    {
        std::cerr << "default oscillation()\n";
    }

    oscillation(std::string file_name): AngleHistory(file_name)//, dangle()
    {
        std::cout << "oscillation(file_name) constructor\n";
            //std::cout << angle.size() << std::endl;
            //std::cout << dangle.size() << std::endl;
        dangle.reserve(this->size());

        double h = time[1] - time[0];

        ///dangle calc
            dangle.push_back(0);
            for(int i = 1; i < size()-1; ++i)
            {
                dangle.push_back(angle[i-1] - angle[i+1]);
                dangle[i] /= 2.0*h;
            }
            dangle.push_back(0);

        ///ddangle calc
            ddangle.push_back(0);
            for(int i = 1; i < size()-1; ++i)
            {
                ddangle.push_back(dangle[i-1] - dangle[i+1]);
                ddangle[i] /= 2.0*h;
            }
            ddangle.push_back(0);

    }

    ~oscillation(){
        dangle.clear();
    }


    double get_angle(int i){
    return angle[i];}

    double get_dangle(int i){
    return dangle[i];}

    double get_ddangle(int i){
    return dangle[i];}


    void print(){
        for(size_t i = 0; i < size(); i++){
            std::cerr
                << time[i] << "\t"
                << angle[i] << "\t"
                << dangle[i] << "\t"
                << ddangle[i] << "\n";
        }

    }

    void write(std::string file_name)
    {
        std::cout << "oscillation write(file_name)\n";
        this->info();
        //angle_history.print();
        std::ofstream fout(file_name);
        for(size_t i = 0; i < this->size(); i++){
            fout
                << time[i] << "\t"
                << angle[i] << "\t"
                << dangle[i] << "\t"
                << ddangle[i] << "\n";
            //std::cout << time[i] << std::endl;
        }
        fout.close();
    }

    size_t size()override {return angle.size();}

    void info() override {
        std::cout << "oscillation object \nsize = ";
        std::cout << this->size() << std::endl;
    }
};

namespace periods
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
        int i = 0;

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

    }

}

int main(){
//    AngleHistory A;
//        A.load_row("C:/PostGrad/phd/2016-2021-M3/2016-2021-M3/3942-ist.txt");
//        A.info();
//        //A.print();
        //oscillation B;
        oscillation A(DataPath + "3942-ist.txt");
        //A.print();
          //  A.info();
        //A.write(DataPath + "3942_o.txt");
        periods::PlotScript(DataPath + "Plotter", periods::calculate_periods(A,""));

}
