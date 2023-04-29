#pragma once

#include <vector>
#include <string>
#include <fstream>

#include <boost/optional.hpp>
#include <tuple>

#include <gsl/gsl_fit.h>

#include "gnusl_wrapper/approximation/basic.h"

namespace approximation::linnear
{

    struct ApproxResult : public ApproxResultBasic
    {
        ApproxResult()
        {
            m_method = "linnear";
            m_method = "f(x) = c1*x + c0";
        }

        // ApproxResult(const ApproxResult &rv) : xf(rv.xf), yf(rv.yf), yfErr(rv.yfErr)
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
                    fout << xOfMiddle << "\t" << xf.at(i) << "\t" << yf.at(i) << "\t" << yfErr.at(i) << "\n";

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

        double xOfMiddle;
    };

    struct ApproxResultVector // todo base on AngleHistory Refactored
    {

        void emplace_back(ApproxResult approxResult) // todo refactor
        {
            approxResultVector.emplace_back(approxResult);
        }

        size_t size() const
        {
            return approxResultVector.size();
        }

        void save(const std::string &fileName)
        {
            std::cout << "saving ApproxResult to file " << fileName << "\n";

            std::ofstream fout(fileName + "_coeff");

            int i = 0;
            for (const auto &res : approxResultVector)

                // if (res.c1 > -1 && res.c1 < 0) // fixme for testing
                fout << i++ << "\t" << res.xOfMiddle << "\t" << res.c0 << "\t" << res.c1 << "\n";

            fout.close();
        }

        std::vector<ApproxResult> approxResultVector;
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
                            &c0, &c1,
                            &cov00, &cov01, &cov11, &chisq);

            //for (size_t i = 0; i < n; ++i)
            //    std::cout << "n:" << n << " x: " << x[i] << " y: " << y[i] << " w: " << w[i] << "\n";

            ApproxResult result;

            result.c0 = c0;
            result.c1 = c1;
            result.cov00 = cov00;
            result.cov01 = cov01;
            result.cov11 = cov11;
            result.chisq = chisq;

            result.xOfMiddle = x[n / 2];

            result.reserve(n);

            /*std::cout << "c0: " << c0 << "\n"
            << "c1: " << c1 << "\n"
            << "cov00: " << cov00 << "\n"
            << "cov01: " << cov01 << "\n"
            << "cov11: " << cov11 << "\n"
            << "chisq: " << chisq << "\n";*/

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

            /*std::cout << "DUFUQ\n";
                    std::cout << "c0: " << c0 << "\n"
                    << "c1: " << c1 << "\n"
                    << "cov00: " << cov00 << "\n"
                    << "cov01: " << cov01 << "\n"
                    << "cov11: " << cov11 << "\n"
                    << "chisq: " << chisq << "\n";*/

            return std::make_tuple(0, result);
        }

    private:
        double *x;
        double *y;
        double *w;
        size_t n;
    };

    inline std::tuple<int, ApproxResult> approximate(const std::vector<double> &inputDataX,
                                              const std::vector<double> &inputDataY,
                                              const boost::optional<std::vector<double>> &err = boost::optional<std::vector<double>>())
    {
        // std::cout << "approximation::linnear::approximate performing, xSize: "
        //          << inputDataX.size() << ", ySize:" << inputDataY.size() << "\n";

        // for (size_t i = 0; i < inputDataX.size(); ++i)
        //    std::cout << inputDataX.at(i) << "\t" << inputDataY.at(i) << "\n";

        ProceedApproximation pA(inputDataX, inputDataY, err);

        return pA.act();
    }

    namespace
    {
        bool correctRangesAndWarn(const size_t maxSize,
                                  size_t &fromIndex,
                                  size_t &finishIndex,
                                  size_t &windowWidth,
                                  size_t &stepSize)
        {
            std::cout << "correctRangesAngWarnentry()\n";

            bool wasCorrected = false;

            if (fromIndex > maxSize)
            {
                std::cerr << "\tfromIndex > max size, fromIndex to 0, "
                          << fromIndex << " > " << maxSize << "\n";

                fromIndex = 0;

                std::cerr << "\tfromIndex has beeng set to: " << fromIndex << "\n";

                wasCorrected = true;
            }

            if (finishIndex > maxSize)
            {
                std::cerr << "\tfinishIndex > max size, setting finishIndex to maxSize, "
                          << finishIndex << " > " << maxSize << "\n";

                finishIndex = maxSize;

                std::cerr << "\tfinishIndex has beeng set to: " << finishIndex << "\n";

                wasCorrected = true;
            }

            if (fromIndex + windowWidth > maxSize)
            {
                std::cerr << "fromIndex + windowWidth > max size, setting windowWidth to maximum possible, "
                          << fromIndex + windowWidth << " > " << maxSize << "\n";

                windowWidth = maxSize - fromIndex;

                std::cerr << "\twindowWidth has beeng set to: " << windowWidth << "\n";

                wasCorrected = true;
            }

            if (fromIndex + stepSize > maxSize)
            {
                std::cerr << "\tfromIndex + stepSize > max size, setting stepSize to maximum possible, "
                          << fromIndex + stepSize << " > " << maxSize << "\n";

                stepSize = maxSize - fromIndex;

                std::cerr << "\tstepSize has beeng set to: " << stepSize << "\n";

                wasCorrected = true;
            }

            return wasCorrected;
        }
        
    } // namespace

    inline std::tuple<int, ApproxResultVector> windowApproximation(const size_t indexFromData,
                                          const size_t indexToData,
                                          const size_t windowSize,
                                          const size_t stepSize,
                                          const std::vector<double> &x,
                                          const std::vector<double> &y)
    {
        int resultCode; // todo badCode

        size_t FROM_INDEX_X = indexFromData; // 400; // начинаем с

        size_t FINISH_INDEX = indexToData; // 10000; // кончаем

        size_t WINDOW_WIDTH = windowSize; // 100; // апрроксимация на
        size_t STEP_SIZE = stepSize;      // WINDOW_WIDTH * 1;

        // based on data
        const size_t MAX_SIZE_DATA = x.size();

        correctRangesAndWarn(MAX_SIZE_DATA, FROM_INDEX_X, FINISH_INDEX, WINDOW_WIDTH, STEP_SIZE);

        approximation::linnear::ApproxResultVector approxResultVector;

        {
            // PARAMETERS FOR STEP
            size_t stepFromIdx = FROM_INDEX_X;
            size_t stepToIdx = stepFromIdx + WINDOW_WIDTH;

            approximation::linnear::ApproxResult approxResult;

            for (; stepToIdx < FINISH_INDEX;)
            {
                std::vector<double> subX(x.begin() + stepFromIdx, x.begin() + stepToIdx);
                std::vector<double> subY(y.begin() + stepFromIdx, y.begin() + stepToIdx);

                // prepare for approx
                for (auto &itm : subY)
                {
                    itm = abs(itm);
                }

                // AngleHistory a(subX, subY);
                // a.write("approxThis");



                std::tie(resultCode, approxResult) = approximation::linnear::approximate(subX, subY);

                approxResultVector.emplace_back(approxResult);
                std::cout << "approxResultVector.size: " << approxResultVector.size() << "\n";

                stepFromIdx += STEP_SIZE;
                stepToIdx += STEP_SIZE;
            } // loop
        }

        return std::make_tuple(resultCode, approxResultVector);
    }

} // approximation::linnear
