#pragma once

#include <iostream>
#include <math.h>

#include <vector>

const double PI = 3.14159265359;

namespace filters
{

    namespace inner
    {
        // https://ru.dsplib.org/content/filter_butter_ap/filter_butter_ap.html
        int calculateBatterworthOrder(const double Rp,
                                      const double Rs,
                                      const double wp,
                                      const double ws)
        {
            double ep = sqrt(pow(10, Rp/10) - 1);
            double es = sqrt(pow(10, Rs/10) - 1);

            std::cout << "ep = " << ep << "\n"
                        << "es = " << es << "\n";

            double N = log(es/ep)/log(ws/wp);

            std::cout << "N = " << N << "\n";

            N = floor(N+1);

            std::cout << "trunc N = " << N << "\n";

            return N;
        }
    }


    namespace
    {
        bool isEven(int k)
        {
            if ( double(k/2) == k/2)
            {
                std::cout << "is even\n";
                return true;
            }
                std::cout << "is odd\n";
            return false;
        }
    }

    inline std::vector<double> calculatePolynimCoeff(const double Rp,
                        const double Rs,
                        const double wp,
                        const double ws)
    {
        // N = 2*L + 1
        const int N = inner::calculateBatterworthOrder(Rp, Rs, wp, ws);

        int r  = 0, L = 0;
        if (isEven(N))
        {
            r = 0;
            L = N/2;
        }
        else
        {
            r = 1;
            L = (N - 1)/2;
        }


        double ep = sqrt(pow(10, Rp/10) - 1);

        std::cout << "r = " << r << "\n";
        std::cout << "L = " << L << "\n";

        double alpha = pow(ep, -(1.0/N));

        std::cout << "alpha = " << alpha << "\n";

        std::vector<double> theta;

        theta.reserve(L);

        for (int i = 1; i <= L; i++)
            theta.push_back( (2.0*i-1.0)/2.0/N * PI);


        int k = 1;
        for (auto th : theta)
        {

            std::cout << "theta" << k << " = " << th << ", sin(theta" << k << ") = " << sin(th) << "; ";
        }
        std::cout << "\n";

        return theta;
    }

    namespace batterworth_3
    {

        class Batterworth3Filter
        {

        public:

            double b1, b2, b3, a0, a1, a5, zz, Psi, Psi2, Beta;

            double A[7];
            double B[7];


            void BP_Filter(float F0)
            {
                Psi = cos(F0*PI)*zz;

                Psi2 = Psi*Psi;

                A[1] = Psi*a1;
                A[2] = a0*((Psi2 + 1.0)*(3.0 + b1) - b2-3.0*b3);
                A[3] = a0*Psi*(Psi2+6.-2*b2);
                A[4] = a0*((Psi2+1.0)*(3.0-b1)-b2+3.0*b3);
                A[5] = Psi*a5;
            }

            void BP_Filter_Init(double Fs)
            {
                double z = Fs*0.5*PI;

                Beta = tan(z);

                zz = -2.0/cos(z);

                b1 = 2.0*Beta;

                b3 = Beta*Beta;
                b2 = 2.0*b3;
                b3 = Beta*b3;

                a0 = 1.0/(1.0 + b1 + b2 + b3);
                a1 = a0 *(3.0 + 2.0*b1 + b2);
                a5 = a0 *(3.0 - 2.0*b1 + b2);

                A[0] = 1.0;
                A[6] = a0*(1.0 - b1 + b2 - b3);

                B[0] = b3*a0;
                B[1] = 0.0;
                B[2] = -3.0*B[0];
                B[3] = 0.0;
                B[4] = -B[2];
                B[5] = 0.0;
                B[6] = -B[0];
            }

            void main(int N, double Fs)
            {
                int i;

                //N = 10;
                //Fs = 0.1;  // полоса фильтра


                BP_Filter_Init(Fs);

                for (i = 1; i < N; i++)
                {
                    BP_Filter(0.3 + i/20); // F0

                    std::cout << "#" << i << "\t"
                                << b1 << "\t"
                                << b2 << "\t"
                                << b3 << "\t"
                                << a0 << "\t"
                                << a1 << "\t"
                                << a5 << "\t"
                                << zz << "\t"
                                << Psi << "\t"
                                << Beta << "\n";
                }
            }


            void doJob(const std::vector<double>& input, std::vector<double>& output)
            {
                std::cout << "butterworth::doJob() " << input.size() << "\n";

                output.reserve(input.size());

                for (int i = 0; i < 6; i++)
                    output.push_back(input.at(i));


                std::cout << "A = "
                    << A[0] << " "
                    << A[1] << " "
                    << A[2] << " "
                    << A[3] << " "
                    << A[4] << " "
                    << A[5] << "\n"

                << "B = " << " "
                    << B[0] << " "
                    << B[1] << " "
                    << B[2] << " "
                    << B[3] << " "
                    << B[4] << " "
                    << B[5] << "\n";

                for (size_t n = 6; n < input.size(); n++)
                {
                    //std::cout << "n = " << n << "\t" << input.size() << "\t" << output.size() << "\n";

                    output.push_back(
                         B[0]*input[n]   +
                         B[1]*input[n-1] +
                         B[2]*input[n-2] +
                         B[3]*input[n-3] +
                         B[4]*input[n-4] +
                         B[5]*input[n-5] +
                         B[6]*input[n-6] -

                         A[1]*output[n-1] -
                         A[2]*output[n-2] -
                         A[3]*output[n-3] -
                         A[4]*output[n-4] -
                         A[5]*output[n-5] -
                         A[6]*output[n-6]);
                }

            }

        };
    }

}
