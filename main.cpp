#include <iostream>
#include "CppPTSLClient.h"

int main() {
    PTSLC_CPP::ClientConfig config {
        "localhost:31416",
    };

    auto client = std::make_unique<PTSLC_CPP::CppPTSLClient>(config);

    std::cout << "This Works!" << std::endl;

    return 0;
}