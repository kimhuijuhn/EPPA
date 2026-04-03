#include "toggle_play_state.h"
#include "ptsl_helpers.h"

#include <iostream>

using namespace std;
using namespace PTSLC_CPP;

void DoTogglePlayState(CppPTSLClient& client)
{
    CppPTSLRequest req(CommandId::CId_TogglePlayState);

    auto rsp = client.SendRequest(req).get();
    if (rsp.GetStatus() == CommandStatusType::TStatus_Completed)
        cout << "TogglePlayState: OK\n";
    else
    {
        cerr << "TogglePlayState failed\n";
        PrintErrors(rsp);
    }
}
