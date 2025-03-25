#pragma once

#include <vector>
#include <ostream>

// todo make template class
class FourVector
{
public:
    FourVector()
    {
    }

    FourVector(const size_t size) : m_d0(size), m_d1(size), m_d2(size), m_d3(size)
    {
    }

    FourVector(const std::vector<double> &v0,
               const std::vector<double> &v1,
               const std::vector<double> &v2,
               const std::vector<double> &v3)
        : m_d0(v0), m_d1(v1), m_d2(v2), m_d3(v3){};

    friend std::ostream &operator<<(std::ostream &foutSource, const FourVector &data)
    {
        // for (size_t i = data.size(); i < data.size()/2 + data.size()%2; ++i)
        for (size_t i = 0; i < data.size(); ++i)
        {
            // std::cout << data.m_0.at(i) << "\t" << data.m_1.at(i) << "\t" << data.m_2.at(i) << "\t" << data.m_3.at(i) << "\n";
            foutSource << data.m_d0.at(i)<< " " << data.m_d1.at(i) << " " << data.m_d2.at(i) << " " << data.m_d3.at(i) << "\n";
        }
        return foutSource;
    }

    void reserve(const size_t size)
    {
        m_d0.reserve(size);
        m_d1.reserve(size);
        m_d2.reserve(size);
        m_d3.reserve(size);
    }

    void push_back(double d0, double d1, double d2, double d3)
    {
        m_d0.push_back(d0);
        m_d1.push_back(d1);
        m_d2.push_back(d2);
        m_d3.push_back(d3);
    }

    size_t size() const
    {
        return m_d1.size();
    }

    std::vector<double> m_d0;
    std::vector<double> m_d1;
    std::vector<double> m_d2;
    std::vector<double> m_d3;
};
