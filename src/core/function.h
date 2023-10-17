#pragma once

#include <cmath>
#include <algorithm>
#include <vector>

namespace
{
    struct Values
    {
        Values(double _x, double _y) : x(_x), y(_y), relCmpTolerance(0.0001)
        {
        }

        bool operator<(const Values &r) const
        {
            return x < r.x;
        }

        bool operator==(const Values &r) const
        {
            if (r.x != 0)
                return std::abs(x/r.x - 1) < relCmpTolerance;
            else
                return x == r.x;
        }

        double x;
        double y;
        double relCmpTolerance;
    };

    class Soreter
    {
        Soreter(std::vector<double> xV, std::vector<double> yV)
        {
            for (int i = 0; i < xV.size(); i++)
            {
                values.push_back(Values(xV.at(i), yV.at(i)));
            }
        }

        std::vector<Values> values;
    };
}

class Function
{
public:
    Function() : m_domain(std::vector<double>()), m_codomain(std::vector<double>())
    {
    }

    // todo check is sizes equal
    Function(const std::vector<double> &domain,
             const std::vector<double> &codomain)
        : m_domain(domain),
          m_codomain(codomain)
    {
        if (m_domain.size() != m_codomain.size())
            throw "cant construct Function with different domain and codomain sizes!";
    }

    Function(const Function &d) : m_domain(d.m_domain), m_codomain(d.m_codomain)
    {
    }

    virtual ~Function(){};

    virtual Function &operator=(const Function &d)
    {
        m_domain = d.m_domain;
        m_codomain = d.m_codomain;

        return *this;
    }

    virtual std::vector<double> getDomain() const { return m_domain; };
    virtual std::vector<double> getCodomain() const { return m_codomain; };

    /// STL-like
    virtual size_t size() const { return m_domain.size(); }; // fixme both should have same size

    virtual double getDomain(size_t i) const
    {
        return m_domain.at(i);
    }

    virtual double getCodomain(size_t i) const
    {
        return m_codomain.at(i);
    }

    virtual void clear()
    {
        m_domain.clear();
        m_codomain.clear();
    }

    virtual void shrink_to_fit()
    {
        m_domain.shrink_to_fit();
        m_codomain.shrink_to_fit();
    }

    virtual void reserve(const size_t sizeToReserve)
    {
        m_domain.reserve(sizeToReserve);
        m_codomain.reserve(sizeToReserve);
    }

    virtual bool empty() const
    {
        return m_domain.empty();
    }

    virtual void push_back(const double domain, const double codomain)
    {
        m_domain.push_back(domain);
        m_codomain.push_back(codomain);
    }

    virtual void insert(const std::vector<double> domain, const std::vector<double> codomain)
    {
        m_domain.insert(m_domain.begin(), domain.begin(), domain.end());
        m_codomain.insert(m_codomain.begin(), codomain.begin(), codomain.end());
    }

    virtual void insert(size_t indexTo,
                        std::vector<double>::const_iterator domainFrom,
                        std::vector<double>::const_iterator domainTo,
                        std::vector<double>::const_iterator codomainFrom,
                        std::vector<double>::const_iterator codomainTo)
    {
        const size_t actualIndexTo = (indexTo < m_domain.size() ? indexTo : m_domain.size());

        m_domain.insert(m_domain.begin() + actualIndexTo, domainFrom, domainTo);
        m_codomain.insert(m_codomain.begin() + actualIndexTo, codomainFrom, codomainTo);
    }

    // FUNC // move to helpers
    void domainAdd(const double value)
    {
        for (auto &valDomain : m_domain)
            valDomain += value;
    }

    void codomainAdd(const double value)
    {
        for (auto &valCodomain : m_codomain)
            valCodomain += value;
    }

    void domainMultiply(const double value)
    {
        for (auto &valDomain : m_domain)
            valDomain *= value;
    }

    void codomainMultiply(const double value)
    {
        for (auto &valCodomain : m_codomain)
            valCodomain *= value;
    }

    // todo move somewhere?
    void codomainLog()
    {
        for (auto &valCodomain : m_codomain)
            valCodomain = log(valCodomain);
    }

    virtual void info() const
    {
        std::cout << "Function object:\n"
                  << "\tm_domain.size(): " << m_domain.size() << "\n"
                  << "\tm_codomain.size(): " << m_codomain.size() << "\n";
    }

protected:
    // move to private ??
    std::vector<double> m_domain;
    std::vector<double> m_codomain;

private:
};

inline Function sortFunction(Function func)
{
    // Soreter sorter(func.getDomain(), func.getCodomain());
    std::vector<Values> valuesVect;

    for (int i = 0; i < func.size(); i++)
    {
        valuesVect.push_back(Values(func.getDomain(i), func.getCodomain(i)));
    }
    
    const int initialSizeBeforeUnique = valuesVect.size();

    sort(valuesVect.begin(), valuesVect.end());

    auto last = std::unique(valuesVect.begin(), valuesVect.end());

    valuesVect.erase(last, valuesVect.end());

    const int sizeAfterDuplicateRmoval = valuesVect.size();

    if (initialSizeBeforeUnique != sizeAfterDuplicateRmoval)
        std::cout << "SIZE befor and after sortAndUnique changed " 
                  << initialSizeBeforeUnique << " " << sizeAfterDuplicateRmoval << "\n";    

    Function resultFnction;

    for (const auto &v : valuesVect)
    {
        resultFnction.push_back(v.x, v.y);
    }

    return resultFnction;
}
