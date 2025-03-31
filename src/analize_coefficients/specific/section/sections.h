#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <exception>

#include "analize_coefficients/specific/section/section.h"
#include "oscillation/oscillation_basic.h"


class Sections
{
public:
    Sections()
    {}

    Sections(std::shared_ptr<Oscillation> oscillationPtr, const double sectionAngleStep) 
        : m_oscillationPtr(oscillationPtr),
          m_sectionAngleStep(sectionAngleStep)
    {
        if (oscillationPtr->angleBegin() == oscillationPtr->angleEnd())
            throw std::range_error("sections throws, input angle is empty!");

        m_maxAngle           = *std::max_element(oscillationPtr->angleBegin(), oscillationPtr->angleEnd());
        m_minAngle           = *std::min_element(oscillationPtr->angleBegin(), oscillationPtr->angleEnd());
        m_sectionBorderValue = int(std::min(std::abs(m_maxAngle), std::abs(m_minAngle)) / m_sectionAngleStep) * m_sectionAngleStep;
    }

    void calculate()
    {
        double sectionAngle = -m_sectionBorderValue;

        while (sectionAngle <= m_sectionBorderValue)
        {
            {
                Section sectionAsc;
                sectionAsc.calculateSection(m_oscillationPtr, sectionAngle, Section::ASCENDING);
                m_sectrionsVector.push_back(sectionAsc);
            }

            {
                Section sectionDesc;
                sectionDesc.calculateSection(m_oscillationPtr, sectionAngle, Section::DESCENDING);
                m_sectrionsVector.push_back(sectionDesc);
            }

            sectionAngle += m_sectionAngleStep;
        }

        // for specific target angle get from ascending, get from descending ang calculate
        /*for (const auto& section : m_sectrionsVector)
        {
            Function funcDdangleOndangle;

            for (int i = 0; i < section.size()-1; i++)
            {
                double dangle = (section.getDangle(i+1) + section.getDangle(i)) / 2.0;
                double ddangleOndangle = (section.getDdangle(i+1) - section.getDdangle(i)) / (section.getDangle(i+1) - section.getDangle(i));
                
                funcDdangleOndangle.push_back(dangle, ddangleOndangle);
            }

            m_ddangleOndangleVector.push_back(funcDdangleOndangle);
        }*/
    }

    std::tuple<bool, double, double, std::vector<Section>, std::vector<Function>> getData() const
    {
        return std::make_tuple(true, m_minAngle, m_maxAngle, m_sectrionsVector, m_ddangleOndangleVector);
    }

    double sectionAngleStep() const
    {
        return m_sectionAngleStep;
    }

private:
    std::vector<Section> m_sectrionsVector;
    std::vector<Function> m_ddangleOndangleVector;
    double m_maxAngle;
    double m_minAngle;
    double m_sectionBorderValue;
    double m_sectionAngleStep;

    std::shared_ptr<Oscillation> m_oscillationPtr;
};

/*inline std::tuple<bool, double, double, std::vector<Section>>
calculateSectionVector(const Oscillation &oscillation, const double sectionAngleStep)
{

    const double maxAngle = *std::max_element(oscillation.angleBegin(), oscillation.angleEnd());
    const double minAngle = *std::min_element(oscillation.angleBegin(), oscillation.angleEnd());
    const int sectionBorderValue = int(std::min(std::abs(maxAngle), std::abs(minAngle)) / sectionAngleStep) * sectionAngleStep;

    // descriptionStream << "Сечения в промежутке [" << -sectionBorderValue << ", " << sectionBorderValue << "]"
    //                   << std::endl;
    std::vector<Section> sectionVector;

    for (int sectionAngle = -sectionBorderValue; sectionAngle <= sectionBorderValue;)
    {
        // descriptionStream << "Рассчет методом сечений для угла " << sectionAngle << " градусов\n";
        // std::cout << "sectionAngle = " << sectionAngle << "\n";

        bool isOk = false;

        {
            Section sectionAsc;
            sectionAsc.calculateSection(oscillation, sectionAngle, Section::ASCENDING);
            sectionVector.push_back(sectionAsc);
        }
        {
            Section sectionDesc;
            sectionDesc.calculateSection(oscillation, sectionAngle, Section::DESCENDING);
            sectionVector.push_back(sectionDesc);
        }

        sectionAngle += sectionAngleStep;
    }

    return std::make_tuple(true, minAngle, maxAngle, sectionVector);
}*/
