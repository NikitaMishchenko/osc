#pragma once

#include <ostream>
#include <vector>

namespace helpers
{
    // vectorAdd
    // vectorMultiply
    // vectorFunction

    // vectorSave
    template<class T>
    void saveToFile(std::ostream& outSource, const std::vector<T>& data)
    {
        for (const auto& d : data)
        {
            outSource << d << "\n";   
        }
    }
    // vectorLoad
    // vector 
    

} // namespace helpers


