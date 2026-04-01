#include <iostream>
#include "CppPTSLClient.h"

int main() {
    PTSLC_CPP::ClientConfig config {
        "localhost:31416",
    };

    auto client = std::make_unique<PTSLC_CPP::CppPTSLClient>(config);
    client->LaunchProTools();
    std::cout << "I can launch Pro Tools now!" << std::endl;

    return 0;
}