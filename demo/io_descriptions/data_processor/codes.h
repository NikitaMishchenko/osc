#pragma once

#include <string>

namespace io::codes
{
    enum ProcessorErrorCodes
    {
        POSITIVE = 0,
        NEGATIVE,
        FAILED_LOAD_ANGLE_HISTORY,
        FAILED_LOAD_FLOW,
        FAILED_LOAD_MODEL
    };

    inline std::string toString(int errCode)
    {
        switch (errCode)
        {
        case POSITIVE:
            return std::string("POSITIVE(" + std::to_string(POSITIVE) + ")");
        
        case NEGATIVE:
            return std::string("NEGATIVE(" + std::to_string(NEGATIVE) + ")");

        case FAILED_LOAD_ANGLE_HISTORY:
            return std::string("FAILED_LOAD_ANGLE_HISTORY(" + std::to_string(FAILED_LOAD_ANGLE_HISTORY) + ")");

        case FAILED_LOAD_FLOW:
            return std::string("FAILED_LOAD_FLOW(" + std::to_string(FAILED_LOAD_FLOW) + ")");

        case FAILED_LOAD_MODEL:
            return std::string("FAILED_LOAD_MODEL(" + std::to_string(FAILED_LOAD_MODEL) + ")");

        default:
            return "UNKNOWN_ERROR_CODE";
        }
    }

} // namespace io::codes
