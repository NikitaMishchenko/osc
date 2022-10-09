#pragma once

#include <iostream>
#include <fstream>

//#include </home/mishnic/CPP_Projects/fftw-3.3.10/fftw-3.3.10/api/fftw3.h>
#include <fftw3.h>
//#include <rfftw.h>
#include <math.h>
#include <vector>

#include "../oscillation/oscillation_basic.h"

const double PI = 3.14159265359;

namespace fftw
{
    void fftw_complex_to_vectors(fftw_complex *data, size_t dataSize, std::vector<double> &real, std::vector<double> &img)
    {
        std::cout << "fftw_complex_to_vectors() "
                  << "_real.size() = " << real.size() << " _img.size() = " << img.size() << std::endl;

        for (int i = 0; i < dataSize; i++)
        {
            real.at(i) = data[i][0];
            img.at(i) = data[i][1];
        }
    }

    void vectors_to_fftw_complex()
    {
        std::cout << "empty function\n";
    }

    void real_vector_to_fftw_complex(const std::vector<double> &_real, fftw_complex *data, size_t dataSize)
    {
        std::cout << "real_vector_to_fftw_complex() dataSize = " << dataSize << "\n";

        for (int i = 0; i < dataSize; i++)
        {
            data[i][0] = _real.at(i);
            data[i][1] = 0;
        }
    }

    void realVectorToFftwComplex(std::vector<double>::const_iterator signalBegin,
                                 std::vector<double>::const_iterator signalEnd,
                                 fftw_complex *data)
    {
        std::cout << "realVectorToFftwComplex()\n";

        int i = 0;
        for (auto it = signalBegin; it != signalEnd; ++it)
        {
            data[i][0] = *it;
            data[i][1] = 0;
            ++i;
        }
    }

}

namespace osc
{
    namespace fftw
    {
        struct Spectrum
        {

            Spectrum() : amplitude(std::vector<double>()), real(std::vector<double>()), img(std::vector<double>())
            {}

            Spectrum(size_t size) : amplitude(std::vector<double>(size)),
                                    real(std::vector<double>(size)),
                                    img(std::vector<double>(size))
            {}

            // todo refactor. For not overwride such methods it's better to make vector of ***, not *** of vectors
            void reserve(size_t sizeToReserve)
            {
                amplitude.reserve(sizeToReserve);
                real.reserve(sizeToReserve);
                img.reserve(sizeToReserve);
            }

            void clear()
            {
                amplitude.clear();
                real.clear();
                img.clear();
            }

            int64_t size() const 
            {
                return amplitude.size();
            }

            std::vector<double> amplitude;
            std::vector<double> real;
            std::vector<double> img;
        };

        ::osc::fftw::Spectrum fftwComplexToVectors(fftw_complex *data, size_t dataSize)
        {
            std::cout << "fftwComplexToVectors()\n";

            ::osc::fftw::Spectrum spectrum(dataSize);

            for (size_t i = 0; i < dataSize; i++)
            {
                spectrum.real.at(i) = data[i][0];
                spectrum.img.at(i) = data[i][1];
                spectrum.amplitude.at(i) = sqrt(spectrum.real.at(i) * spectrum.real.at(i) + spectrum.img.at(i) * spectrum.img.at(i));
            }

            return spectrum;
        }

        osc::fftw::Spectrum perfomFftw(std::vector<double>::const_iterator signalBegin, std::vector<double>::const_iterator signalEnd)
        {
            //std::cout << "performFftw entry()\n";

            fftw_complex *in, *out;
            fftw_plan p;

            const int signalSize = signalEnd - signalBegin;

            // ALLOC buffs
            in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * signalSize);
            out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * signalSize);

            ::fftw::realVectorToFftwComplex(signalBegin, signalEnd, in);

            p = fftw_plan_dft_1d(signalSize, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

            fftw_execute(p);

            osc::fftw::Spectrum spectrum;

            spectrum = fftwComplexToVectors(out, signalSize);

            //std::cout << "spectrum.size() = " << spectrum.amplitude.size() << std::endl;
            //std::cout << "spectrum.capacity() = " << spectrum.amplitude.capacity() << std::endl;

            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);

            return spectrum;
        }

        // custom iterator?
        osc::fftw::Spectrum perfomFftw(AngleHistory inputSignal)
        {
            std::cout << "performFftw entry()\n";

            fftw_complex *in, *out;
            fftw_plan p;

            in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * inputSignal.size());
            // fftw::generate_signal(in, 0.01, N);

            ::fftw::real_vector_to_fftw_complex(inputSignal.getAngle(), in, inputSignal.size());

            out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * inputSignal.size());

            p = fftw_plan_dft_1d(inputSignal.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);

            fftw_execute(p); /* repeat as needed */

            // std::vector<double> sp_real(real_signal.size());
            // std::vector<double> sp_img(real_signal.size());

            osc::fftw::Spectrum spectrum(inputSignal.size());

            ::fftw::fftw_complex_to_vectors(out, inputSignal.size(), spectrum.real, spectrum.img);

            for (size_t i = 0; i < inputSignal.size(); i++)
            {
                spectrum.amplitude.at(i) = sqrt(spectrum.real.at(i) * spectrum.real.at(i) + spectrum.img.at(i) * spectrum.img.at(i));

                //_spectrum.real.at(i) = _spectrum.real.at(i);

                //_spectrum.img.at(i) = _spectrum.img.at(i);
            }

            std::cout << "spectrum.size() = " << spectrum.amplitude.size() << std::endl;
            std::cout << "spectrum.capacity() = " << spectrum.amplitude.capacity() << std::endl;

            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);

            return spectrum;
        }
    }
}

namespace fftw_example
{

    void generate_signal(fftw_complex *signal, double freq, uint8_t length)
    {
        std::cout << "Signal generation\n";

        for (int i = 0; i < length; ++i)
        {
            signal[i][0] = sin(freq * i) + sin(freq * 2.0 * i);
            signal[i][1] = 0.0;
        }

        std::cout << "Signal saving\n";

        std::ofstream fout("input");
        for (int i = 0; i < length; ++i)
            fout << i << "\t" << signal[i][0] << "\t" << signal[i][1] << std::endl;
        fout.close();
    }

    void func(uint8_t N)
    {
        // const uint8_t N = 1024*16;

        fftw_complex *in, *out;
        fftw_plan p;

        in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
        generate_signal(in, 0.01, N); /// to do colab with Oscillation make signal addition

        for (int i = 0; i < N; ++i)
            std::cout << i << "\t" << in[i][0] << "\t" << in[i][1] << std::endl;

        out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
        p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(p); /* repeat as needed */

        std::ofstream fout("fout");
        for (int i = 0; i < N; ++i)
            fout << out[i][0] << "\t" << out[i][1] << std::endl;
        fout.close();

        fftw_complex *out2;
        out2 = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

        fftw_plan p2;
        p2 = fftw_plan_dft_1d(N, out, out2, FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_execute(p2);

        std::ofstream fout3("fout2");
        for (int i = 0; i < N; ++i)
            fout3 << out2[i][0] << "\t" << out2[i][1] << std::endl;
        fout3.close();

        fftw_destroy_plan(p);
        fftw_destroy_plan(p2);
        fftw_free(in);
        fftw_free(out);
        fftw_free(out2);
    }

} // namespace fftw
