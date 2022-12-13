#pragma once

#include <fstream>

#include <string>

#include "src/oscillation/oscillation_basic.h"

namespace oscillation_files
{

    inline bool cut_file(const std::string& input_file, const double from_time, const double to_time, const std::string& output_file)
    {
        Oscillation A(input_file);

        std::cout << "File loaded. Size of input file = " << A.size() << std::endl;

        Oscillation R;

        std::vector<double>::const_iterator _it = A.timeBegin();

        while( *_it <= from_time && _it != A.timeEnd())
            _it++;

        // начальный отсчет
        size_t first_st = _it - A.timeBegin();
            std::cout << "from it value = " << *_it << std::endl;

        // последний отсчет
        while( *_it <= to_time && _it != A.timeEnd())
            _it++;

        //std::vector<double>::const_iterator last = _it;
        size_t last_st = _it - A.timeBegin();
            std::cout << "to it value = " << *_it << std::endl;

        //std::cout << first_st << "\t" << last_st << "\tsize=" << A.size() <<"\n";


        R.insertAllFieldsFromTo(A, first_st, last_st);

        /*R.time.insert(R.timeBegin(), A.timeBegin()+first_st, A.timeBegin()+last_st);
        R.angle.insert(R.angleBegin(), A.angleBegin()+first_st, A.angleBegin()+last_st);
        R.dangle.insert(R.dangleBegin(), A.dangleBegin()+first_st, A.dangleBegin()+last_st);
        R.ddangle.insert(R.ddangleBegin(), A.ddangleBegin()+first_st, A.ddangleBegin()+last_st);*/

        R.write(output_file);
        return true;
    }

    inline void cut_raw_file(const std::string& input_file, int lines_count_to_skip, const double from_time, const double to_time, const std::string& output_file)
    {
        std::ifstream fin(input_file);
        std::ofstream fout(output_file);


        std::string sbuff;
        int line_counter = 0;
        while(!fin.eof())
        {
            std::getline(fin, sbuff);
            line_counter++;

            if(line_counter > lines_count_to_skip)
                fout << sbuff;

        }

        fout.close();
        fin.close();


        cut_file(output_file, from_time, to_time, output_file);
    }

} // namespace oscillation_files
