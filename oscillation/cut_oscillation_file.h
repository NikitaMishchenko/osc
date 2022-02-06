#pragma once

#include <fstream>

#include <string>

#include "../oscillation_basic.h"

namespace Oscillation_files
{

    bool cut_file(const std::string& input_file, const double from_time, const double to_time, const std::string& output_file)
    {
        Oscillation A(input_file);

        std::cout << "size of input file = " << A.size() << std::endl;

        Oscillation R;

        std::vector<double>::const_iterator _it = A.time.begin();

        while( *_it < from_time && _it != A.time.end())
            _it++;

        //std::vector<double>::const_iterator first = _it;
        size_t first_st = _it - A.time.begin();
            std::cout << "from it value = " << *_it << std::endl;

        while( *_it < to_time && _it != A.time.end())
            _it++;

        //std::vector<double>::const_iterator last = _it;
        size_t last_st = _it - A.time.begin()+1;
            std::cout << "to it value = " << *_it << std::endl;

        R.time.insert(R.time.begin(), A.time.begin()+first_st, A.time.begin()+last_st);
        R.angle.insert(R.angle.begin(), A.angle.begin()+first_st, A.angle.begin()+last_st);
        R.dangle.insert(R.dangle.begin(), A.dangle.begin()+first_st, A.dangle.begin()+last_st);
        R.ddangle.insert(R.ddangle.begin(), A.ddangle.begin()+first_st, A.ddangle.begin()+last_st);

        std::cout << "size_of_cutt_ file = " << R.size() << std::endl;

        R.write(output_file);

        return true;
    }

    void cut_raw_file(const std::string& input_file, int lines_count_to_skip, const double from_time, const double to_time, const std::string& output_file)
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
} // namespace Oscillation_files
