#pragma once

#include <string>

#include "wt_flow.h"

namespace wt_flow
{
    bool parsePTLfile(const std::string& fileName, Flow& flowData, const int maxTestCountsScinceAnglePolling = 999);
}
