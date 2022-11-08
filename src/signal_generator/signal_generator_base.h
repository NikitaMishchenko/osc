#pragma once

#include <fstream>

#include <boost/optional.hpp>

#include "oscillation/angle_history.h"
#include "operation_types.h"

namespace signal_generator
{

    // todo to helpers time to index
    /**
     * Realisation of basic operations
     * int getIndex(double time){std::find() return index };
     *  std::tuple<int, int> getTimeRangeIndex(double timeForm, double timeTod);
     * std::tuple<iterator, iterator>(double timeFrom, timeTo);
     *
     * */

    class SignalGenerator : public AngleHistory
    {
    public:
        SignalGenerator() : AngleHistory()
        {
        }

        // SignalGenerator(double timeStep, size_t size) : m_codomain(std::vector<double>(size){1.0}))

        SignalGenerator(const AngleHistory &angleHistory) : AngleHistory(angleHistory)
        {
        }

        virtual ~SignalGenerator()
        {
        }

        SignalGenerator *makeConstantSignal(const DataForConstantSignal &data)
        {
            return makeConstantSignal(data.amplitude, data.size, data.timeStep);
        }

        SignalGenerator *makeConstantSignal(double amplitude, size_t size, double timeStep)
        {
            std::cout << "makeNoSignal:"
                      << " size: " << size
                      << ", timeStep: " << timeStep << ")\n";

            Function::reserve(size);

            for (size_t i = 0; i < size; ++i)
            {
                m_domain.emplace_back(i * timeStep);
                m_codomain.emplace_back(amplitude);
            }

            return this;
        }

        SignalGenerator *multiplyHarmonic(const DataForMultiplyAddHarmonic &data)
        {
            return multiplyHarmonic(data.indexFrom,
                                    data.indexTo,
                                    data.amplitude,
                                    data.w,
                                    data.phase);
        }

        SignalGenerator *multiplyHarmonic(boost::optional<size_t> indexFrom,
                                          boost::optional<size_t> indexTo,
                                          double amplitude,
                                          double w,
                                          double phase)
        {
            std::cout << "multiplyHarmonic(), ";

            indexFrom = (indexFrom ? indexFrom.get() : 0);
            indexTo = (indexTo ? indexTo.get() : size());

            std::cout << "indexFrom: " << indexFrom.get()
                      << ", indexTo: " << indexTo.get()
                      << ", amplitude: " << amplitude
                      << ", w: " << w
                      << ", phase: " << phase
                      << "\n";

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
                m_codomain.at(i) *= amplitude * sin(w * m_domain.at(i) + phase);

            return this;
        };

        SignalGenerator *addHarmonic(const DataForMultiplyAddHarmonic &data)
        {
            return addHarmonic(data.indexFrom,
                               data.indexTo,
                               data.amplitude,
                               data.w,
                               data.phase);
        }

        SignalGenerator *addHarmonic(boost::optional<size_t> indexFrom,
                                     boost::optional<size_t> indexTo,
                                     double amplitude,
                                     double w,
                                     double phase)
        {
            std::cout << "addHarmonic(), ";

            indexFrom = (indexFrom ? indexFrom.get() : 0);
            indexTo = (indexTo ? indexTo.get() : size());

            std::cout << "indexFrom: " << indexFrom.get()
                      << ", indexTo: " << indexTo.get()
                      << ", amplitude: " << amplitude
                      << ", w: " << w
                      << ", phase: " << phase
                      << "\n";

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
                m_codomain.at(i) += amplitude * sin(w * m_domain.at(i) + phase);

            return this;
        };

        SignalGenerator *addSlopeLinnear(const DataForSlopeLinnear &data)
        {
            return addSlopeLinnear(data.indexFrom,
                                   data.indexTo,
                                   data.slopeA,
                                   data.slopeB);
        }

        SignalGenerator *addSlopeLinnear(boost::optional<size_t> indexFrom,
                                         boost::optional<size_t> indexTo,
                                         double slopeA,
                                         double slopeB)
        {
            std::cout << "addSlopeLinnear(), ";

            indexFrom = (indexFrom ? indexFrom.get() : 0);
            indexTo = (indexTo ? indexTo.get() : size());

            std::cout << "indexFrom: " << indexFrom.get()
                      << ", indexTo: " << indexTo.get()
                      << ", slopeA: " << slopeA
                      << ", slopeB: " << slopeB << "\n";

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
                m_codomain.at(i) += slopeA * m_domain.at(i) + slopeB;

            return this;
        }

        SignalGenerator *multiplySlopeLinnear(const DataForSlopeLinnear &data)
        {
            return multiplySlopeLinnear(data.indexFrom,
                                        data.indexTo,
                                        data.slopeA,
                                        data.slopeB);
        }

        SignalGenerator *multiplySlopeLinnear(boost::optional<size_t> indexFrom,
                                              boost::optional<size_t> indexTo,
                                              double slopeA,
                                              double slopeB)
        {
            std::cout << "multiplySlopeLinnear(), ";

            indexFrom = (indexFrom ? indexFrom.get() : 0);
            indexTo = (indexTo ? indexTo.get() : size());

            std::cout << "indexFrom: " << indexFrom.get()
                      << ", indexTo: " << indexTo.get()
                      << ", slopeA: " << slopeA
                      << ", slopeB: " << slopeB << "\n";

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
                m_codomain.at(i) *= slopeA * m_domain.at(i) + slopeB;

            return this;
        }

        SignalGenerator *amplitudeDecreaseLinnear(const DataForSlopeLinnear &data)
        {
            return amplitudeDecreaseLinnear(data.indexFrom,
                                            data.indexTo,
                                            data.slopeA,
                                            data.slopeB);
        }

        SignalGenerator *amplitudeDecreaseLinnear(boost::optional<size_t> indexFrom,
                                                  boost::optional<size_t> indexTo,
                                                  double slopeA,
                                                  double slopeB)
        {
            std::cout << "amplitudeDecreaseLinnear(), ";

            indexFrom = (indexFrom ? indexFrom.get() : 0);
            indexTo = (indexTo ? indexTo.get() : size());

            std::cout << "indexFrom: " << indexFrom.get()
                      << ", indexTo: " << indexTo.get()
                      << ", slopeA: " << slopeA
                      << ", slopeB: " << slopeB << "\n";

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
            {
                int amplDirection = (m_codomain.at(i) > 0 ? +1 : -1);
                m_codomain.at(i) *= double(amplDirection) * slopeA * m_domain.at(i) + slopeB; // todo remove slope b
            }
            return this;
        }

        SignalGenerator *scaleAngle(DataForScale data)
        {
            return scaleAngle(data.indexFrom,
                              data.indexTo,
                              data.scaleFactor);
        }

        // todo
        SignalGenerator *scaleAngle(boost::optional<size_t> indexFrom,
                                    boost::optional<size_t> indexTo,
                                    double scaleFactor)
        {
            indexFrom = (indexFrom ? indexFrom.get() : 0);
            indexTo = (indexTo ? indexTo.get() : size());

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
                m_codomain.at(i) *= scaleFactor * (m_codomain.at(i));

            return this;
        };

        SignalGenerator *scaleTime(DataForScale data)
        {
            return scaleTime(data.indexFrom,
                             data.indexTo,
                             data.scaleFactor);
        }

        SignalGenerator *scaleTime(boost::optional<size_t> indexFrom,
                                   boost::optional<size_t> indexTo,
                                   double scaleFactor)
        {
            indexFrom = (indexFrom ? indexFrom.get() : 0);
            indexTo = (indexTo ? indexTo.get() : size());

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
                m_domain.at(i) *= scaleFactor * (m_domain.at(i));

            return this;
        }

        SignalGenerator *randomNoize(double noizeAmplitude, double noizeFrequency); // random

        void save(const std::string &fileName) const
        {
            std::ofstream fout(fileName);

            for (size_t i = 0; i < size(); ++i)
            {
                fout << m_domain.at(i) << "\t" << m_codomain.at(i) << "\n";
            }

            fout.close();
        }

        void info() const
        {
            std::cout << "SignalGenerator object\n"
                      << "\t"
                      << "m_domain size: " << m_domain.size() << "\n"
                      << "\t"
                      << "m_codomain size: " << m_codomain.size() << "\n";
        }

        AngleHistory getAngleHistory() const
        {
            AngleHistory angleHistory = static_cast<AngleHistory>(*this);

            return angleHistory;
        }

    private:
    };

} // signal_generator
