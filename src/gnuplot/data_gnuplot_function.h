#pragma once

#include <string>

namespace gnuplot
{

    class DataGnuplotFunction
    {
    public:
        std::string getFunction();
    private:
        std::string m_function;
    };

} // namespace gnuplot
