#include "ProToolsController.h"

#include "../AppConfig.h"

#include <PTSLC_CPP/CppPTSLClient.h>
#include <PTSLC_CPP/CppPTSLCommon.h>
#include <PTSLC_CPP/CppPTSLRequest.h>
#include <PTSLC_CPP/CppPTSLResponse.h>

#include <nlohmann/json.hpp>

#include <iostream>

using json = nlohmann::json;
using namespace PTSLC_CPP;

namespace EPPA {

ProToolsController::ProToolsController(const AppConfig& config)
{
    ClientConfig clientConfig {
        config.ptslAddress,
        Mode::Mode_ProTools,
        SkipHostLaunch::SHLaunch_Yes  // EPPA assumes Pro Tools is already running
    };
    mClient = std::make_unique<CppPTSLClient>(clientConfig);
}

ProToolsController::~ProToolsController() = default;

bool ProToolsController::Connect()
{
    const json body = {
        { "company_name",     "EPPA" },
        { "application_name", "Expression Pedal Playback Assistant" }
    };

    CppPTSLRequest request(CommandId::CId_RegisterConnection, body.dump());
    const CppPTSLResponse response = mClient->SendRequest(std::move(request)).get();

    if (response.GetStatus() != TaskStatus::TStatus_Completed) {
        std::cerr << "[EPPA] RegisterConnection failed: " << response.GetResponseErrorJson() << "\n";
        return false;
    }

    mIsConnected = true;
    std::cout << "[EPPA] Connected to Pro Tools. Session ID: " << mClient->GetSessionId() << "\n";
    return true;
}

bool ProToolsController::IsConnected() const
{
    return mIsConnected;
}

bool ProToolsController::IsPlaying()
{
    CppPTSLRequest request(CommandId::CId_GetTransportState);
    const CppPTSLResponse response = mClient->SendRequest(std::move(request)).get();

    if (response.GetStatus() != TaskStatus::TStatus_Completed) {
        std::cerr << "[EPPA] GetTransportState failed.\n";
        return false;
    }

    const json body  = json::parse(response.GetResponseBodyJson());
    const std::string state = body.value("current_setting", "TState_Unknown");

    // Both the current and the deprecated alias for the playing state are checked
    // in case the Pro Tools version returns the older enum string.
    return (state == "TState_TransportPlaying" || state == "TS_TransportPlaying");
}

bool ProToolsController::TogglePlay()
{
    CppPTSLRequest request(CommandId::CId_TogglePlayState);
    const CppPTSLResponse response = mClient->SendRequest(std::move(request)).get();

    if (response.GetStatus() != TaskStatus::TStatus_Completed) {
        std::cerr << "[EPPA] TogglePlayState failed.\n";
        return false;
    }
    return true;
}

bool ProToolsController::Play()
{
    if (!IsPlaying()) {
        std::cout << "[EPPA] Starting playback.\n";
        return TogglePlay();
    }
    return true;  // already playing — nothing to do
}

bool ProToolsController::Stop()
{
    if (IsPlaying()) {
        std::cout << "[EPPA] Stopping playback.\n";
        return TogglePlay();
    }
    return true;  // already stopped — nothing to do
}

std::string ProToolsController::GetSelectedTrackName()
{
    const json requestBody = {
        { "track_filter_list", json::array({
            { { "filter", "TLFilter_Selected" }, { "is_inverted", false } }
        })},
        { "pagination_request", { { "limit", 1 }, { "offset", 0 } } }
    };

    CppPTSLRequest request(CommandId::CId_GetTrackList, requestBody.dump());
    const CppPTSLResponse response = mClient->SendRequest(std::move(request)).get();

    if (response.GetStatus() != TaskStatus::TStatus_Completed) {
        std::cerr << "[EPPA] GetTrackList failed.\n";
        return "";
    }

    const json body      = json::parse(response.GetResponseBodyJson());
    const auto trackList = body.value("track_list", json::array());

    if (trackList.empty()) {
        std::cerr << "[EPPA] No selected track found.\n";
        return "";
    }

    return trackList[0].value("name", "");
}

bool ProToolsController::SetTrackSolo(const std::string& trackName, bool enabled)
{
    if (trackName.empty()) return false;

    const json requestBody = {
        { "track_names", json::array({ trackName }) },
        { "enabled",     enabled }
    };

    CppPTSLRequest request(CommandId::CId_SetTrackSoloState, requestBody.dump());
    const CppPTSLResponse response = mClient->SendRequest(std::move(request)).get();

    if (response.GetStatus() != TaskStatus::TStatus_Completed) {
        std::cerr << "[EPPA] SetTrackSoloState failed for track: " << trackName << "\n";
        return false;
    }
    return true;
}

bool ProToolsController::SoloSelectedTrack()
{
    // Always clean up any previously soloed track before soloing a new one.
    if (!mSoloedTrack.empty()) {
        SetTrackSolo(mSoloedTrack, false);
        mSoloedTrack.clear();
    }

    const std::string trackName = GetSelectedTrackName();
    if (trackName.empty()) return false;

    if (SetTrackSolo(trackName, true)) {
        mSoloedTrack = trackName;
        std::cout << "[EPPA] Soloed track: " << trackName << "\n";
        return true;
    }
    return false;
}

bool ProToolsController::UnsoloCurrentTrack()
{
    if (mSoloedTrack.empty()) return true;  // nothing to unsolo

    const bool result = SetTrackSolo(mSoloedTrack, false);
    if (result) {
        std::cout << "[EPPA] Unsoloed track: " << mSoloedTrack << "\n";
        mSoloedTrack.clear();
    }
    return result;
}

} // namespace EPPA
