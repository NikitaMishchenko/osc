#include <gtest/gtest.h>

#include "fft/fftw_impl.h"

TEST(TestFFT, fft1)
{
    fftw_example::func(10000);

}