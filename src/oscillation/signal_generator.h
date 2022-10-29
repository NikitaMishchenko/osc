#pragma once

#include <fstream>

#include <boost/optional.hpp>

#include "angle_history.h"

namespace signal_generator
{
    // todo to helpers time to index
    /**
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

        SignalGenerator *makeConstantSignal(double amplitude, size_t size, double timeStep)
        {
            std::cout << "makeNoSignal(size: " << size << ", timeStep: " << timeStep << ")\n";

            Function::reserve(size);
            Function::shrink_to_fit();

            info();

            for (size_t i = 0; i < size; ++i)
            {
                m_domain.emplace_back(i * timeStep);
                m_codomain.emplace_back(amplitude);
            }

            info();

            return this;
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

            std::cout << "indexFrom: " << indexFrom.get() << " ,indexTo: " << indexTo.get() << "\n";

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
                m_codomain.at(i) *= amplitude * sin(w * m_domain.at(i) + phase);

            return this;
        };

        SignalGenerator *addHarmonic(boost::optional<size_t> indexFrom,
                                     boost::optional<size_t> indexTo,
                                     double amplitude,
                                     double w,
                                     double phase)
        {
            std::cout << "multiplyHarmonic(), ";

            indexFrom = (indexFrom ? indexFrom.get() : 0);
            indexTo = (indexTo ? indexTo.get() : size());

            std::cout << "indexFrom: " << indexFrom.get() << " ,indexTo: " << indexTo.get() << "\n";

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
                m_codomain.at(i) += amplitude * sin(w * m_domain.at(i) + phase);

            return this;
        };

        SignalGenerator* slopeLinnear(boost::optional<size_t> indexFrom,
                                     boost::optional<size_t> indexTo,
                                     double slopeA,
                                     double slopeB)
        {
            std::cout << "slopeLinnear(), ";

            indexFrom = (indexFrom ? indexFrom.get() : 0);
            indexTo = (indexTo ? indexTo.get() : size());

            std::cout << "indexFrom: " << indexFrom.get() << " ,indexTo: " << indexTo.get() 
                      << ", slopeA: " <<  slopeA << ", slopeB: " << slopeB << "\n";

            for (size_t i = indexFrom.get(); i < indexTo.get(); ++i)
                m_codomain.at(i) *= slopeA*m_domain.at(i) + slopeB; 

            return this;

        }

        // todo
        SignalGenerator *scaleAmplitude(boost::optional<size_t> indexFrom,
                                        boost::optional<size_t> indexTo,
                                        double scaleFactor)
        {
            for (size_t i = (indexFrom ? indexFrom.get() : 0); i < (indexTo ? indexTo.get() : size()); ++i)
                m_codomain.at(i) *= scaleFactor * (m_codomain.at(i));

            return this;
        };

        SignalGenerator *scaleTime(boost::optional<size_t> indexFrom,
                                   boost::optional<size_t> indexTo,
                                   double scaleFactor)
        {
            for (size_t i = (indexFrom ? indexFrom.get() : 0); i < (indexTo ? indexTo.get() : size()); ++i)
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

    private:
    };

} // signal_generator
