#pragma once

#include <vector>
#include <string>
#include <fstream>

#include <boost/optional.hpp>
#include <tuple>

#include <gsl/gsl_fit.h>

namespace linnear_approximation
{

    struct ApproxResult
    {
        //ApproxResult()
        //{}

        //ApproxResult(const ApproxResult &rv) : xf(rv.xf), yf(rv.yf), yfErr(rv.yfErr)
        //{}

        void reserve(const size_t n)
        {
            xf.reserve(n);
            yf.reserve(n);
            yfErr.reserve(n);
        }

        void push_back(double rXf, double rYf, double rYfErr)
        {
            xf.push_back(rXf);
            yf.push_back(rYf);
            yfErr.push_back(rYfErr);
        }

        void save(const std::string &fileName)
        {
            std::cout << "saving ApproxResult to file " << fileName << "\n";

            std::ofstream fout(fileName + "_coeff");
            fout << c0 << "\t" << c1 << "\n";
            fout.close();

            // todo if probeResult
            {
                std::ofstream fout(fileName + "_probe");

                for (size_t i = 0; i < xf.size(); ++i)
                {
                    fout << xf.at(i) << "\t" << yf.at(i) << "\t" << yfErr.at(i) << "\n";

                    // std::cout << xf.at(i) << "\t" << yf.at(i) << "\t" << yfErr.at(i) << "\n";
                }

                fout.close();
            }
        }

        /*friend std::ostream &operator<<(std::ostream &out, const ApproxResult &D)
        {

                out << "c0: " << c0 << "\n"
                    << "c1: " << c1 << "\n"
                    << "cov00: " << cov00 << "\n"
                    << "cov01: " << cov01 << "\n"
                    << "cov11: " << cov11 << "\n"
                    << "chisq: " << chisq << "\n";

                return out;
        }*/

        double c0;
        double c1;
        double cov00, cov01, cov11, chisq;

        std::vector<double> xf;
        std::vector<double> yf;
        std::vector<double> yfErr;
    };

    class ProceedApproximation
    {
    public:
        ProceedApproximation(const std::vector<double> &toX,
                             const std::vector<double> &toY,
                             const boost::optional<std::vector<double>> &toW)
        {
            n = toX.size();
            x = new double[n];
            y = new double[n];
            w = new double[n];

            for (size_t i = 0; i < n; ++i)
            {
                x[i] = toX.at(i);
                y[i] = toY.at(i);
            }

            if (toW)
            {
                for (size_t i = 0; i < n; ++i)
                    w[i] = toW->at(i);
            }
            else
            {
                for (size_t i = 0; i < n; ++i)
                    w[i] = 10000000; // todo make it propper
            }
        }

        ~ProceedApproximation()
        {
            delete[] x;
            delete[] y;
            delete[] w;
        }

        // @returns errCode, ApproxResult
        std::tuple<int, ApproxResult> act()
        {
            double c0, c1, cov00, cov01, cov11, chisq;

            gsl_fit_wlinear(x, 1, w, 1, y, 1, n,
                            &c0, &c1, &cov00, &cov01, &cov11,
                            &chisq);

            for (size_t i = 0; i < n; ++i)
                std::cout << x[i] << "\t" << y[i] << "\t" << w[i] << "\n";

            ApproxResult result;
            result.c0 = c0;
            result.c1 = c1;
            result.reserve(n);

                    std::cout << "c0: " << c0 << "\n"
                    << "c1: " << c1 << "\n"
                    << "cov00: " << cov00 << "\n"
                    << "cov01: " << cov01 << "\n"
                    << "cov11: " << cov11 << "\n"
                    << "chisq: " << chisq << "\n";

            for (int i = -30; i < 130; i++)
            {
                double xf = x[0] + (i / 100.0) * (x[n - 1] - x[0]);
                double yf, yf_err;
                gsl_fit_linear_est(xf,
                                   c0, c1,
                                   cov00, cov01, cov11,
                                   &yf, &yf_err);

                // std::cout << xf << "\t" << yf << "\t" << yf_err << "\n";

                result.push_back(xf, yf, yf_err);
            }

            std::cout << "DUFUQ\n";
                    std::cout << "c0: " << c0 << "\n"
                    << "c1: " << c1 << "\n"
                    << "cov00: " << cov00 << "\n"
                    << "cov01: " << cov01 << "\n"
                    << "cov11: " << cov11 << "\n"
                    << "chisq: " << chisq << "\n";

            return std::make_tuple(0, result);
        }

    private:
        double *x;
        double *y;
        double *w;
        size_t n;
    };

    std::tuple<int, ApproxResult> approximate(const std::vector<double> &inputDataX,
                                              const std::vector<double> &inputDataY,
                                              const boost::optional<std::vector<double>> &err = boost::optional<std::vector<double>>())
    {
        std::cout << "linnear_approximation::approximate performing, xSize: "
                  << inputDataX.size() << "ySize:" << inputDataY.size() << "\n";

        for (size_t i = 0; i < inputDataX.size(); ++i)
        {
            std::cout << inputDataX.at(i) << "\t" << inputDataY.at(i) << "\n";
        }

        ProceedApproximation pA(inputDataX, inputDataY, err);

        return pA.act();
    }

} // linnear_approximation