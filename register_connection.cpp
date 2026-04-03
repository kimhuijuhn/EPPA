#include "register_connection.h"
#include "ptsl_helpers.h"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using namespace PTSLC_CPP;
using json = nlohmann::json;

void DoRegisterConnection(CppPTSLClient& client)
{
    json body;
    body["company_name"]     = "MyCompany";
    body["application_name"] = "PTSL Sandbox";

    CppPTSLRequest req(CommandId::CId_RegisterConnection, body.dump());

    auto rsp = client.SendRequest(req).get();
    if (rsp.GetStatus() == CommandStatusType::TStatus_Completed)
    {
        auto rspBody = json::parse(rsp.GetResponseBodyJson());
        cout << "RegisterConnection: session id = "
             << rspBody.value("session_id", "") << "\n";
    }
    else
    {
        cerr << "RegisterConnection failed\n";
        PrintErrors(rsp);
    }
}
