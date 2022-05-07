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

    void print()
    {
        std::cout << "Model:\n"
            << "\tname = " << m_name << "\n"
            << "\ts = " << m_refSquare << "\n"
            << "\tl = " << m_refLength << "\n"
            << "\tI = " << m_momentOfInertia << "\n";
    }

    /*
    *   todo .model file
    */
    bool loadFile(const std::string& fileName)
    {
        std::cout << "parsing file " << fileName << "...\n";
        std::ifstream fin(fileName);

        if (!fin.is_open())
        {
            std::cerr << "model: can't open file " << fileName << "\n";
            return false;
        }

        std::string buff_s;
        fin >> buff_s >> buff_s >> m_name
            >> buff_s >> buff_s >> m_refSquare
            >> buff_s >> buff_s >> m_refLength
            >> buff_s >> buff_s >> m_momentOfInertia;

        fin.close();

        return true;
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
