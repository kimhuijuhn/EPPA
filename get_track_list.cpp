#include "get_track_list.h"
#include "ptsl_helpers.h"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using namespace PTSLC_CPP;
using json = nlohmann::json;

void DoGetTrackList(CppPTSLClient& client)
{
    json reqBody;
    reqBody["is_filter_list_additive"] = true;
    reqBody["track_filter_list"] = json::array({
        { {"filter", 1}, {"is_inverted", false} }   // 1 = TLFilter_All
    });
    reqBody["pagination_request"] = { {"limit", 1000}, {"offset", 0} };

    CppPTSLRequest req(CommandId::CId_GetTrackList, reqBody.dump());

    auto rsp = client.SendRequest(req).get();
    if (rsp.GetStatus() == CommandStatusType::TStatus_Completed)
    {
        auto body = json::parse(rsp.GetResponseBodyJson());
        auto tracks = body.value("track_list", json::array());
        cout << "Track list (" << tracks.size() << " tracks):\n";
        for (const auto& t : tracks)
            cout << "  [" << t.value("index", -1) << "] " << t.value("name", "") << "\n";
    }
    else
    {
        cerr << "GetTrackList failed\n";
        PrintErrors(rsp);
    }
}
