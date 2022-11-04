#pragma once

#include <string>
#include <utility>
#include <tuple>

#include <boost/optional.hpp>

#include "oscillation/oscillation_basic.h"
#include "fft/fftw_impl.h"
#include "periods/periods_base.h"
#include "oscillation/cut_oscillation_file.h"
#include "oscillation/wt_oscillation.h"
#include "flow/parse_ptl.h"
#include "flow/wt_flow.h"
#include "analize_coefficients/dynamic_coefficients.h"
#include "errcodes.h"
#include "pendulum/pendulum_analisys.h"
#include "gnusl_proc/linnear_approximation.h"
#include "analize_coefficients/dynamic_coefficients.h"

#include "signal_generator/signal_generator_base.h"
#include "signal_generator/signal_generator_manager.h"
//#include "signal_generator/signal_generator_operation.h"

#include "signal_generator/operations_queue.h"

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
    ErrorCodes performProcedurePeriods(const std::string &fileName,
                                       const std::string &fileName2,
                                       const std::vector<double> &extraArguments)
    {
        std::cout << "periods procedure performing...\n";

        std::ifstream fin();

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

    ErrorCodes performProcedureFlow(const std::string &fileName)
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

    ErrorCodes performProcedureCutFile(const std::string &initialFile,
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

    ErrorCodes performProceduereProcessingWtData(const std::string &fileName)
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

        wtTest.getMz();
        wtTest.saveMzData(fileName + "_mz");
        // todo check

        wtTest.getMzAmplitudeIndexes();
        wtTest.saveMzAmplitudeData(fileName + "_mz_amplitude");

        return SUCCESS;
    };

    ErrorCodes performProcedurePendulum(const std::string &fileName, const double arg1, uint32_t windowStep)
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

    ErrorCodes performProcedureLinnearApproximation(const std::string &fileName)
    {
        AngleHistory angleHistory;

        if (!angleHistory.loadRaw(fileName))
            return FAIL;

        int errCode;
        linnear_approximation::ApproxResult result;

        std::tie(errCode, result) = linnear_approximation::approximate(angleHistory.getTime(), angleHistory.getAngle(), boost::none);

        result.save(fileName + "_approx");

        return SUCCESS;
    }

    std::tuple<int, linnear_approximation::ApproxResultVector>
    performProcedurecCoefficientsFromWindow(const std::string fileName,
                                            const size_t indexFromData,
                                            const size_t indexToData,
                                            const size_t windowSize,
                                            const size_t stepSize)
    {
        std::cout << "performing performProcedureWindow()\n";

        AngleHistory angleHistory;

        int errCode = FAIL;
        linnear_approximation::ApproxResultVector approxResultVector;

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
    std::tuple<int, linnear_approximation::ApproxResultVector>
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
        linnear_approximation::ApproxResultVector approxResultVector;

        if (!angleHistory.loadRaw(fileName))
            return std::make_tuple(errCode, approxResultVector);

        /// PREPARE DATA


        if (moveAngleValue)
            angleHistory.codomainAdd(moveAngleValue.get());

        // actually no need not used
        pendulum::removeOffscale(angleHistory);
        
        // for pendulum only
        ///pendulum::remove0Harmonic(angleHistory);

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


    ErrorCodes testFunc()
    {
        std::cout << "performing testFunc\n";

        /*signal_generator::SignalGenerator signalGenerator;
        
        signalGenerator.info();

        (&signalGenerator)->makeConstantSignal(1.0, 5000, 0.001)
                          ->multiplyHarmonic(boost::none, boost::none, 10, 10, 0.0)
                          ->addHarmonic(boost::none, boost::none, 1, 100, 0.0)
                          ->slopeLinnear(boost::none, boost::none, -10, 10);

        signalGenerator.info();

        signalGenerator.save("generatedSignal");*/
        
        /*
        fixme refactored
        SignalGeneratorManager mngr;

        mngr.loadSignalConfiguaration("signalToGenerate");

        std::cout << "doWork started\n";
        std::cout << "**************************************************\n";

        mngr.doWork();*/

        OperationsQueue operationQueue;

        operationQueue.loadOperationsConfiguration("signalToGenerate");

        operationQueue.performOperations();


        return FAIL;
    }

    /*
    TODO
     фильтрация, опции, нормальный функционал, деление на логику и работу с файлами
    */

} // basic_procedures
