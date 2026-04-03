#include "host_ready_check.h"
#include "ptsl_helpers.h"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using namespace PTSLC_CPP;
using json = nlohmann::json;

void DoHostReadyCheck(CppPTSLClient& client)
{
    CppPTSLRequest req(CommandId::CId_HostReadyCheck);

    auto rsp = client.SendRequest(req).get();
    if (rsp.GetStatus() == CommandStatusType::TStatus_Completed)
    {
        auto body = json::parse(rsp.GetResponseBodyJson());
        cout << "HostReadyCheck: isHostReady = " << boolalpha
             << body.value("is_host_ready", false) << "\n";
    }
    else
    {
        cerr << "HostReadyCheck failed\n";
        PrintErrors(rsp);
    }
}
