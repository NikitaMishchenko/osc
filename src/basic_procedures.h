#pragma once

#include <string>
#include <utility>
#include <tuple>
#include <memory>

#include <boost/optional.hpp>

#include "oscillation/oscillation_basic.h"
#include "oscillation/wt_oscillation.h"
#include "fft/fftw_impl.h"
#include "periods/periods_base.h"
#include "io_helpers/cut_oscillation_file.h"
#include "flow/parse_ptl.h"
#include "flow/wt_flow.h"
#include "analize_coefficients/dynamic_coefficients.h"
#include "errcodes.h"
#include "pendulum/pendulum_analisys.h"
#include "gnusl_proc/approximation/linnear.h"
#include "analize_coefficients/dynamic_coefficients.h"

#include "signal_generator/operations_queue.h"

// for testing
#include "io_helpers/naive.h"
#include "filtration/gsl_filters.h"
#include "core/vector_helpers.h"
#include "analize_coefficients/dynamic_coefficient.h"
#include "gnusl_proc/approximation/nonlinear.h"

namespace basic_procedures
{

    /**
     *    Data loaded as Oscillation object. So the the initial data is y and t. And while Oscillation constructed
     *    y' and y'' is calculating.
     *
     *    Basically procedure finds periods of the signal and saves it in specific manner.
     *    Each period splits into forward a backward movement
     *    (todo) after that gnuplot script creating.
     */
    inline ErrorCodes performProcedurePeriods(const std::string &fileName,
                                              const std::string &fileName2,
                                              const std::vector<double> &extraArguments)
    {
        std::cout << "periods procedure performing...\n";

        Oscillation D;
        D.loadFile(fileName);
        if (0 == D.size())
        {
            std::cerr << "Empty Oscillation data! Aborting procedure\n";
            return FAIL;
        }

        std::cout << "extra arguments size = " << extraArguments.size() << std::endl;

        int mergeHalfPeriods = 1;

        if (extraArguments.size() > 1)
        {
            mergeHalfPeriods = extraArguments.at(1);
            std::cout << "mergeHalfPeriods = " << mergeHalfPeriods << "\n";
        }

        std::vector<Oscillation> periodsList;

        periodsList = periods::splitPeriods(D);

        std::cout << "Data split into " << periodsList.size() << "periods\n";

        std::string outputFileName = std::string();

        std::ofstream fout;

        std::ofstream fout2(fileName + ".periods");

        for (size_t i = 0; i < periodsList.size();)
        {
            outputFileName = fileName2 + "_" + std::to_string(i);

            if (mergeHalfPeriods > 1)
            {
                outputFileName += "_" + std::to_string(i + mergeHalfPeriods);
            }

            outputFileName += "_pi";

            fout.open(outputFileName);

            for (int halfperiodsToFile = 0; (halfperiodsToFile < mergeHalfPeriods && i < periodsList.size()); halfperiodsToFile++)
            {
                fout << periodsList.at(i);

                if (i < periodsList.size() - 2)
                {
                    fout2 << periodsList.at(i).getTime(0) << "\t"
                          << periodsList.at(i).getAngle(0) << "\t"
                          << periodsList.at(i).getDangle(0) << "\t"
                          << periodsList.at(i).getDdangle(0) << "\t"
                          << periodsList.at(i + 2).getTime(0) - periodsList.at(i).getTime(0) << "\t"
                          << 1 / (periodsList.at(i + 2).getTime(0) - periodsList.at(i).getTime(0)) << "\n";
                }

                i++;
            }

            fout.close();
        }
        fout2.close();

        return SUCCESS;
    }

    inline ErrorCodes performProcedureFlow(const std::string &fileName)
    {
        wt_flow::Flow flow;

        if (wt_flow::parsePTLfile(fileName + ".ptl", flow, 5))
        {
            flow.saveFile(fileName + ".flow");
            std::cout << "File " << fileName << " parsed: success\n";
            return SUCCESS;
        }

        std::cerr << "File " << fileName << " parsed: failed\n";

        return FAIL;
    }

    inline ErrorCodes performProcedureCutFile(const std::string &initialFile,
                                              const double timeFrom,
                                              const double timeTo,
                                              const std::string &finalFile)
    {
        std::cout << "cut file performing\n";

        if (!oscillation_files::cut_file(initialFile, timeFrom, timeTo, finalFile))
        {
            std::cerr << "cut file failed!/n";
            return FAIL;
        }

        return SUCCESS;
    }

    inline ErrorCodes performProceduereProcessingWtData(const std::string &fileName)
    {
        std::cout << "wt test processing mode performing\n";

        /// FLOW->
        wt_flow::Flow flow;
        if (!flow.loadFile((fileName + "_flow")))
            return FAIL;

        flow.calculateFlow();
        flow.print();
        /// FLOW<-

        /// MODEL->
        Model model;
        if (!model.loadFile((fileName + "_model")))
            return FAIL;

        model.print();
        /// MODEL<-

        /// OSCILLATION->
        Oscillation oscillation;
        if (!oscillation.loadFile(fileName + "_osc"))
            return FAIL;
        /// OSCILLATION<-

        WtOscillation wtTest(oscillation, flow, model);

        wtTest.getMz(); // fixme now it returns mz
        wtTest.saveMzData(fileName + "_mz");
        // todo check

        wtTest.calcAngleAmplitudeIndexes();
        wtTest.saveMzAmplitudeData(fileName + "_mz_amplitude");

        return SUCCESS;
    };

    inline ErrorCodes performProcedurePendulum(const std::string &fileName, const double arg1, uint32_t windowStep)
    {
        AngleHistory angleHistory;

        if (!angleHistory.loadRaw(fileName))
            return FAIL;

        pendulum::removeOffscale(angleHistory);

        pendulum::remove0Harmonic(angleHistory); // todo config file to basic procedures // based on boost::property_tree

        bool noErr;
        std::vector<pendulum::Frequency> freqs;

        if (windowStep)
        {
            std::tie(noErr, freqs) = pendulum::getFrequenciesViaFft(angleHistory,
                                                                    static_cast<uint32_t>(arg1),
                                                                    windowStep); // to config or external params
        }
        else
        {
            std::tie(noErr, freqs) = pendulum::getFrequenciesViaDerevative(angleHistory, arg1); // todo coefficient to params or config
            // std::tie(noErr, freqs) = pendulum::getFrequenciesViaSignal(angleHistory);
        }
        saveFile(fileName + "_freqs", freqs);

        if (!noErr)
            return FAIL;

        return SUCCESS;
    }

    inline ErrorCodes performProcedureLinnearApproximation(const std::string &fileName)
    {
        AngleHistory angleHistory;

        if (!angleHistory.loadRaw(fileName))
            return FAIL;

        int errCode;
        approximation::linnear::ApproxResult result;

        std::tie(errCode, result) = approximation::linnear::approximate(angleHistory.getTime(), angleHistory.getAngle(), boost::none);

        result.save(fileName + "_approx");

        return SUCCESS;
    }

    inline std::tuple<int, approximation::linnear::ApproxResultVector>
    performProcedurecCoefficientsFromWindow(const std::string fileName,
                                            const size_t indexFromData,
                                            const size_t indexToData,
                                            const size_t windowSize,
                                            const size_t stepSize)
    {
        std::cout << "performing performProcedureWindow()\n";

        AngleHistory angleHistory;

        int errCode = FAIL;
        approximation::linnear::ApproxResultVector approxResultVector;

        if (!angleHistory.loadRaw(fileName))
            return std::make_tuple(errCode, approxResultVector);

        // for pendulum only
        pendulum::removeOffscale(angleHistory);
        pendulum::remove0Harmonic(angleHistory);

        WtOscillation wt(angleHistory);

        wt.write(fileName + "_prepared_for_arpx");

        dynamic_coefficients::EqvivalentDamping eqvivalentDamping(wt);

        eqvivalentDamping.prepareAmplitude();

        std::tie(errCode, approxResultVector) = eqvivalentDamping.calcAmplitudeLinnarApproxCoeff(indexFromData,
                                                                                                 indexToData,
                                                                                                 windowSize,
                                                                                                 stepSize);

        return std::make_tuple(ErrorCodes(errCode), approxResultVector);
    }

    /*  TODO
     *   ln(Theta(t)) = (n_c(Theta_sr)*t) + ln(c)
     *   result = c1*t+c0
     *   y = c1*x + c0
     *   m_wtOscillation->getAmplitude
     *   y = ln(amplitude) = ln(m_wtOscillation.getAmplitude())
     *   x = m_wtOscillation.getTime();
     *   approximate (x , y)
     *   c1 = ln(c)
     *   c0 = n_c
     */
    inline std::tuple<int, approximation::linnear::ApproxResultVector>
    performProcedurecDynCoefficientsFromWindowForWtTest(const std::string fileName,
                                                        const size_t indexFromData,
                                                        const size_t indexToData,
                                                        const size_t windowSize,
                                                        const size_t stepSize,
                                                        boost::optional<double> moveAngleValue)
    {
        std::cout << "performing performProcedureWindow()\n";

        AngleHistory angleHistory;

        int errCode = FAIL;
        approximation::linnear::ApproxResultVector approxResultVector;

        if (!angleHistory.loadRaw(fileName))
            return std::make_tuple(errCode, approxResultVector);

        /// PREPARE DATA

        if (moveAngleValue)
            angleHistory.codomainAdd(moveAngleValue.get());

        // actually no need not used
        // fixme pendulum::removeOffscale(angleHistory);

        // for pendulum only
        /// pendulum::remove0Harmonic(angleHistory);

        WtOscillation wt(angleHistory);

        dynamic_coefficients::EqvivalentDamping eqvivalentDamping(wt);

        const bool isWtOscillation = true;
        eqvivalentDamping.prepareAmplitude(isWtOscillation);

        std::tie(errCode, approxResultVector) = eqvivalentDamping.calcAmplitudeLinnarApproxCoeff(indexFromData,
                                                                                                 indexToData,
                                                                                                 windowSize,
                                                                                                 stepSize);

        return std::make_tuple(ErrorCodes(errCode), approxResultVector);
    }

    inline ErrorCodes performProcedureFilterSignalViaGaussSimpleFitler(const std::string &fileNameInput,
                                                                       const std::string &fileNameOutput,
                                                                       const size_t windowSize,
                                                                       const double alphaValue)
    {
        std::cout << "performProcedureFilterSignalViaGaussSimpleFitler() windowSize: " << windowSize << ", aplphaValue: " << alphaValue << "\n";
        // todo proper input
        AngleHistory inputAngleHistory(fileNameInput);

        // todo make it input data
        const size_t GAUSS_FILTER_WINDOW_SIZE = windowSize;
        const double GAUSS_FILTER_ALPHA_VALUE = alphaValue;

        std::vector<double> res = actLinnearGaussFilter(GAUSS_FILTER_WINDOW_SIZE,
                                                        GAUSS_FILTER_ALPHA_VALUE,
                                                        inputAngleHistory.getAngle());

        std::cout << "result size: " << res.size() << "\n";

        std::cout << "sigma = " << double((GAUSS_FILTER_WINDOW_SIZE - 1) / 2.0 / GAUSS_FILTER_ALPHA_VALUE) << "\n";

        AngleHistory result(inputAngleHistory.getTime(), res);

        result.write(fileNameOutput);

        return SUCCESS;
    }

    inline ErrorCodes testFunc()
    {
        std::cout << "performing testFunc from basic procedures\n";

        /*OperationsQueue operationQueue;

        operationQueue.loadOperationsConfiguration("signalToGenerate");

        operationQueue.performOperations();

        operationQueue.getAngleHistory().write("resultSignalGenerator");*/

        /// GAUSS FILTER TESTS

        // Oscillation oscillation;
        // oscillation.loadFile("4471");//, oscillation_helpers::TIME_ANGLE_DANGLE_DDANGLE);

        // approximation::nonlinnear::act();
/*
        std::vector<double> dataX;
        std::vector<double> dataY;
        size_t N = 10000;

        for (int i = 0; i < N; ++i)
        {
            dataX.resize(N);
            dataY.resize(N);

            double dt = 0.1;

            for (int i = 0; i < dataX.size(); ++i)
            {
                dataX.at(i) = dt*i;
                dataY.at(i) = (1.0 + 5 * exp(-1.5 * i * dt));
            }
        }

        AngleHistory angleHistory(dataX, dataY);

        int errCode = 1;
        approximation::nonlinnear::ApproximationResult approximationResult;

        std::tie(errCode, approximationResult) = approximation::nonlinnear::approximate(angleHistory.getTime(), angleHistory.getAngle());
*/
        if (true)
        {
            std::string fileName = "4470";

            /// FLOW->
            wt_flow::Flow flow;
            if (!flow.loadFile((fileName + "_flow")))
                return FAIL;

            // flow.calculateFlow();
            flow.print();
            /// FLOW<-

            /// MODEL->
            Model model;
            if (!model.loadFile((fileName + "_model")))
                return FAIL;

            model.print();
            /// MODEL<-

            Oscillation oscillation;

            oscillation.loadFile(fileName, oscillation_helpers::TIME_ANGLE_DANGLE_DDANGLE);

            std::shared_ptr<WtOscillation> wtTest = std::make_shared<WtOscillation>(oscillation, flow, model);

            std::string fileNameMz = fileName + "_mz";

            // wtTest.saveMzData(fileNameMz);
            //  todo check

            wtTest->calcAngleAmplitudeIndexes();
            // wtTest.getMz();
            // wtTest.saveMzAmplitudeData(fileName + "_mz_amplitude");

            // wtTest.saveMzData(fileNameMz);

            DynamicPitchCoefficient dynamicPitchCoefficient(wtTest);

            bool isOk = false;
            // std::vector<double> dynamicPart;
            // std::vector<double> staticPart;
            // std::vector<double> dynamicCoefficient;

            // std::tie(isOk, dynamicPart, staticPart, dynamicCoefficient) = dynamicPitchCoefficient.doCalculateEquation();

            std::vector<std::vector<double>> output;

            output.push_back(wtTest->getTime());
            output.push_back(wtTest->getAngle());
            output.push_back(wtTest->getDangle());
            output.push_back(wtTest->getDdangle()); // 4

            // output.push_back(dynamicPart); // 5
            // output.push_back(staticPart); // 6
            // output.push_back(dynamicCoefficient); // 7

            dynamicPitchCoefficient.calculate();
            output.push_back(dynamicPitchCoefficient.getAngle());                // 9 5
            output.push_back(dynamicPitchCoefficient.getDangle());               // 10 6
            output.push_back(dynamicPitchCoefficient.getDdangle());              // 11 7
            output.push_back(dynamicPitchCoefficient.getPitchMomentum());        // 12 8
            output.push_back(dynamicPitchCoefficient.getPitchStaticMomentum());  // 13 9
            output.push_back(dynamicPitchCoefficient.getPitchDynamicMomentum()); // 14 10

            dynamicPitchCoefficient.info();

            std::ofstream fout("dynPich");

            writeToFile(fout, output);

            std::vector<double> pickedDynamic1;
            std::vector<double> pickedDynamic2;

            std::tie(pickedDynamic1, pickedDynamic2) = dynamicPitchCoefficient.pickPitchDynamicMomentumAtMaxAplitude();

            std::ofstream fout1("picked");

            pickedDynamic1.resize(std::max(pickedDynamic1.size(), pickedDynamic2.size()));
            pickedDynamic2.resize(std::max(pickedDynamic1.size(), pickedDynamic2.size()));

            for (size_t i = 0; i < pickedDynamic1.size(); ++i)
            {
                fout1 << pickedDynamic1.at(i) << "\t" << pickedDynamic2.at(i) << "\n";
            }

            wtTest->saveMzData("mzdata");

            std::vector<double> amplitude1;
            std::vector<double> amplitude2;

            std::tie(amplitude1, amplitude2) = dynamicPitchCoefficient.getAmplitude();
            
            std::ofstream fout2("amplitude");

            amplitude1.resize(std::max(amplitude1.size(), amplitude2.size()));
            amplitude2.resize(std::max(amplitude1.size(), amplitude2.size()));

            for (size_t i = 0; i < amplitude1.size(); ++i)
            {
                fout2 << amplitude1.at(i) << "\t" << amplitude2.at(i) << "\n";
            }
        }

        return FAIL;
    }

    /*
    TODO
     фильтрация, опции, нормальный функционал, деление на логику и работу с файлами
    */

} // basic_procedures
