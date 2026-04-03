#include "get_session_name.h"
#include "ptsl_helpers.h"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using namespace PTSLC_CPP;
using json = nlohmann::json;

void DoGetSessionName(CppPTSLClient& client)
{
    CppPTSLRequest req(CommandId::CId_GetSessionName);

    auto rsp = client.SendRequest(req).get();
    if (rsp.GetStatus() == CommandStatusType::TStatus_Completed)
    {
        auto body = json::parse(rsp.GetResponseBodyJson());
        cout << "Session name: " << body.value("session_name", "") << "\n";
    }
    else
    {
        cerr << "GetSessionName failed\n";
        PrintErrors(rsp);
    }
}
