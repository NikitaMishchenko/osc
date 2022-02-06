#pragma once



class Model
{
public:

    Model() : m_refSquare(1.0), m_refLength(1.0), m_momentOfInertia(1.0)
    {

    }

    Model(const double& s, const double&   l, const double&  I) : m_refSquare(s), m_refLength(l), m_momentOfInertia(I)
    {
    }

    /// GET
    double getL()
    {
        return m_refLength;
    }

    double getS()
    {
        return m_refSquare;
    }

    double getI()
    {
        return m_momentOfInertia;
    }


    /// TECH
    bool isDefault()
    {
        return m_refSquare == 1.0  && m_refLength == 1.0  && m_momentOfInertia == 1.0;
    }




    /// IO
    friend std::ostream& operator<< (std::ostream& out, const Model& m)
    {
        out << m.m_name << "\t"
            << m.m_refSquare << "\t"
            << m.m_refLength << "\t"
            << m.m_momentOfInertia << "\t"
            << "\n";
        return out;
    }


    /*
    *   todo .model file
    */
    bool loadFromFile(std::string& fileName)
    {
        std::ifstream fin(fileName);

        if (!fin.is_open())
            return false;

        fin >> m_name
            >> m_refSquare
            >> m_refLength
            >> m_momentOfInertia;


        fin.close();
    }



    //////////////////////////////////
    private:

    // geometry
    double m_refSquare;
    double m_refLength;

    // mass properties
    double m_momentOfInertia;
    double m_locationCG; // extra

    // extra
    std::string m_name = "";

};
