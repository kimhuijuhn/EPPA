#pragma once

#include <iostream>
#include "CppPTSLResponse.h"

// Print errors from a CppPTSLResponse.
inline void PrintErrors(const PTSLC_CPP::CppPTSLResponse& rsp)
{
    for (const auto& err : rsp.GetResponseErrorList().errors)
        std::cerr << "  error: " << err->errorMessage << "\n";
}
