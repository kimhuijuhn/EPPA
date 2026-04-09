#pragma once

#include <memory>
#include <string>

namespace PTSLC_CPP { class CppPTSLClient; }

// Wraps the PTSL client and exposes high-level transport and track actions.
// Always queries Pro Tools' live state before acting so the controller stays
// correct even if PT state changes from another input (keyboard, mouse, etc.).
class PlaybackController {
public:
    explicit PlaybackController(const std::string& address = "localhost:31416");
    ~PlaybackController();

    // Connects to Pro Tools and registers EPPA as a client.
    // Must be called before any other method.
    bool Connect();
    bool IsConnected() const;

    // Queries transport state; sends TogglePlayState only if stopped.
    bool Play();

    // Queries transport state; sends TogglePlayState only if playing.
    bool Stop();

    // Solos the first currently selected track. Unsolos the previous one first.
    // Returns the track name on success, empty string if no track is selected.
    std::string SoloSelectedTrack();

    // Unsolos the track stored by the last SoloSelectedTrack() call.
    bool UnsoloCurrentTrack();

private:
    bool IsPlaying();
    bool TogglePlay();
    bool SetTrackSolo(const std::string& trackName, bool enabled);
    std::string GetSelectedTrackName();

    std::unique_ptr<PTSLC_CPP::CppPTSLClient> mClient;
    std::string mAddress;
    std::string mSoloedTrack;
    bool        mIsConnected = false;
};
