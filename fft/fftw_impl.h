#pragma once

#include <iostream>
#include <fstream>

//#include </home/mishnic/CPP_Projects/fftw-3.3.10/fftw-3.3.10/api/fftw3.h>
#include <fftw3.h>
//#include <rfftw.h>
#include <math.h>
#include <vector>

#include "../oscillation_basic.h"


const double PI = 3.14159265359;

std::vector<double> spectrum;

namespace fftw
{
    void fftw_complex_to_vectors(fftw_complex* data, uint8_t data_size, std::vector<double>& _real, std::vector<double>& _img)
    {
        std::cout << "fftw_complex_to_vectors() " << "_real.size() = " << _real.size() << " _img.size() = " << _img.size() << std::endl;


        for(int i = 0; i < data_size; i++)
        {
            _real.at(i) = data[i][0];
            _img.at(i) = data[i][1];
        }
    }

    void vectors_to_fftw_complex()
    {
        std::cout << "empty function\n";
    }

    void real_vector_to_fftw_complex(const std::vector<double>& _real, fftw_complex* data, uint8_t data_size)
    {
        std::cout << "real_vector_to_fftw_complex() " << std::endl;
        for(int i = 0; i < data_size; i++)
        {
            data[i][0] = _real.at(i);
            data[i][1] = 0;
        }
    }
}


namespace osc
{
    namespace fftw
    {
        struct spectrum
        {
            std::vector<double> amplitude;
            std::vector<double> real;
            std::vector<double> img;

            spectrum(size_t _size) : amplitude(_size), real(_size), img(_size)
            {
                //std::vector<double> v(_size);
                //amplitude = real = img = v
            }
        };

        void perfom_fftw(Oscillation real_signal, spectrum& _spectrum)
        {

            fftw_complex *in, *out;
            fftw_plan p;

            in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * real_signal.size());
            //fftw::generate_signal(in, 0.01, N);

            ::fftw::real_vector_to_fftw_complex(real_signal.get_angle(), in, real_signal.size());

            out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * real_signal.size());

            p = fftw_plan_dft_1d(real_signal.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);

            fftw_execute(p); /* repeat as needed */

            //std::vector<double> sp_real(real_signal.size());
            //std::vector<double> sp_img(real_signal.size());


            ::fftw::fftw_complex_to_vectors(out, real_signal.size(), _spectrum.real, _spectrum.img);//sp_real, sp_img);

            _spectrum.amplitude.reserve(real_signal.size());
            _spectrum.real.reserve(real_signal.size());
            _spectrum.img.reserve(real_signal.size());

            for(size_t i = 0; i < real_signal.size(); i++)
            {
                _spectrum.amplitude.at(i) = sqrt(_spectrum.real.at(i)*_spectrum.real.at(i) + _spectrum.img.at(i)*_spectrum.img.at(i));

                //_spectrum.real.at(i) = _spectrum.real.at(i);

                //_spectrum.img.at(i) = _spectrum.img.at(i);
            }

            std::cout << "_spectrum.size() = " << _spectrum.amplitude.size() << std::endl;
            std::cout << "_spectrum.capacity() = " << _spectrum.amplitude.capacity() << std::endl;

            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);
        }
    }
}

namespace fftw_example
{

    void generate_signal(fftw_complex* signal, double freq, uint8_t length)
    {
        std::cout << "Signal generation\n";

        for(int i = 0; i < length; ++i)
        {
            signal[i][0] = sin(freq*i) + sin(freq*2.0*i);
            signal[i][1] = 0.0;
        }


        std::cout << "Signal saving\n";

        std::ofstream fout("input");
        for(int i = 0; i < length; ++i)
            fout << i << "\t" << signal[i][0] << "\t" << signal[i][1] << std::endl;
        fout.close();

    }


    void func(uint8_t N)
    {
        //const uint8_t N = 1024*16;

        fftw_complex *in, *out;
        fftw_plan p;

        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
            generate_signal(in, 0.01, N); ///to do colab with Oscillation make signal addition

        for(int i = 0; i < N; ++i)
            std::cout << i << "\t" << in[i][0] << "\t" << in[i][1] << std::endl;


        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
            p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
                fftw_execute(p); /* repeat as needed */

        std::ofstream fout("fout");
        for(int i = 0; i < N; ++i)
            fout << out[i][0] << "\t" << out[i][1] << std::endl;
        fout.close();


        fftw_complex *out2;
        out2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

        fftw_plan p2;
        p2 = fftw_plan_dft_1d(N, out, out2, FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_execute(p2);

        std::ofstream fout3("fout2");
        for(int i = 0; i < N; ++i)
            fout3 << out2[i][0] << "\t" << out2[i][1] << std::endl;
        fout3.close();


        fftw_destroy_plan(p);
        fftw_destroy_plan(p2);
        fftw_free(in);
        fftw_free(out);
        fftw_free(out2);
    }
} // namespace fftw
