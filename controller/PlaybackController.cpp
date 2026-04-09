#include "PlaybackController.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include "CppPTSLClient.h"

using namespace PTSLC_CPP;
using json = nlohmann::json;

PlaybackController::PlaybackController(const std::string& address)
    : mAddress(address)
{
}

PlaybackController::~PlaybackController() = default;

bool PlaybackController::Connect()
{
    ClientConfig config;
    config.address        = mAddress;
    config.serverMode     = Mode::Mode_ProTools;
    config.skipHostLaunch = SkipHostLaunch::SHLaunch_No;

    mClient = std::make_unique<CppPTSLClient>(config);

    // Register connection
    json body;
    body["company_name"]     = "EPPA";
    body["application_name"] = "Expression Pedal Playback Assistant";

    CppPTSLRequest req(CommandId::CId_RegisterConnection, body.dump());
    auto rsp = mClient->SendRequest(req).get();
    if (rsp.GetStatus() != CommandStatusType::TStatus_Completed) {
        std::cerr << "PlaybackController: RegisterConnection failed\n";
        return false;
    }

    mIsConnected = true;
    return true;
}

bool PlaybackController::IsConnected() const
{
    return mIsConnected;
}

bool PlaybackController::Play()
{
    if (IsPlaying()) return true;
    return TogglePlay();
}

bool PlaybackController::Stop()
{
    if (!IsPlaying()) return true;
    return TogglePlay();
}

std::string PlaybackController::SoloSelectedTrack()
{
    std::string trackName = GetSelectedTrackName();
    if (trackName.empty()) {
        std::cerr << "PlaybackController: no track selected\n";
        return "";
    }

    if (!mSoloedTrack.empty())
        SetTrackSolo(mSoloedTrack, false);

    if (SetTrackSolo(trackName, true)) {
        mSoloedTrack = trackName;
        return trackName;
    }
    return "";
}

bool PlaybackController::UnsoloCurrentTrack()
{
    if (mSoloedTrack.empty()) return true;
    bool ok = SetTrackSolo(mSoloedTrack, false);
    if (ok) mSoloedTrack.clear();
    return ok;
}

// --- private ---

bool PlaybackController::IsPlaying()
{
    CppPTSLRequest req(CommandId::CId_GetTransportState);
    auto rsp = mClient->SendRequest(req).get();
    if (rsp.GetStatus() != CommandStatusType::TStatus_Completed) return false;

    auto body = json::parse(rsp.GetResponseBodyJson());
    std::string state = body.value("current_setting", "");
    return state == "TState_TransportPlaying" || state == "TS_TransportPlaying";
}

bool PlaybackController::TogglePlay()
{
    CppPTSLRequest req(CommandId::CId_TogglePlayState);
    auto rsp = mClient->SendRequest(req).get();
    return rsp.GetStatus() == CommandStatusType::TStatus_Completed;
}

bool PlaybackController::SetTrackSolo(const std::string& trackName, bool enabled)
{
    json body;
    body["track_names"] = json::array({ trackName });
    body["enabled"]     = enabled;

    CppPTSLRequest req(CommandId::CId_SetTrackSoloState, body.dump());
    auto rsp = mClient->SendRequest(req).get();
    return rsp.GetStatus() == CommandStatusType::TStatus_Completed;
}

std::string PlaybackController::GetSelectedTrackName()
{
    json reqBody;
    reqBody["is_filter_list_additive"] = true;
    reqBody["track_filter_list"] = json::array({
        { {"filter", 2}, {"is_inverted", false} }   // 2 = TLFilter_Selected
    });
    reqBody["pagination_request"] = { {"limit", 1}, {"offset", 0} };

    CppPTSLRequest req(CommandId::CId_GetTrackList, reqBody.dump());
    auto rsp = mClient->SendRequest(req).get();
    if (rsp.GetStatus() != CommandStatusType::TStatus_Completed) return "";

    auto body   = json::parse(rsp.GetResponseBodyJson());
    auto tracks = body.value("track_list", json::array());
    if (tracks.empty()) return "";

    return tracks[0].value("name", "");
}
