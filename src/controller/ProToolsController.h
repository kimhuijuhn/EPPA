#pragma once

#include <memory>
#include <string>

// Forward declaration — avoids pulling the full SDK header into every translation unit.
namespace PTSLC_CPP {
    class CppPTSLClient;
}

namespace EPPA {

    struct AppConfig;

    // Wraps the PTSL SDK client and exposes high-level transport and solo actions
    // for use by App in response to pedal zone changes.
    //
    // All transport decisions are made by first querying Pro Tools' live state,
    // so the controller stays correct even if PT state changes from another input
    // (keyboard, mouse, other MIDI device). This is essential for VI users who
    // cannot visually verify transport state.
    class ProToolsController {
    public:
        explicit ProToolsController(const AppConfig& config);
        ~ProToolsController();

        // Connects to Pro Tools via gRPC and registers EPPA as a client.
        // Must be called before any other method.
        // Returns false if Pro Tools is unreachable or registration fails.
        bool Connect();

        bool IsConnected() const;

        // Queries GetTransportState; sends TogglePlayState only if transport is stopped.
        bool Play();

        // Queries GetTransportState; sends TogglePlayState only if transport is playing.
        bool Stop();

        // Queries GetTrackList(selected), solos the first selected track via SetTrackSoloState,
        // and stores the track name internally. Unsolos any previously stored track first.
        bool SoloSelectedTrack();

        // Sends SetTrackSoloState(false) for the track stored by the last SoloSelectedTrack() call.
        // No-op if no track is currently stored.
        bool UnsoloCurrentTrack();

    private:
        // Returns true if Pro Tools reports TState_TransportPlaying.
        bool IsPlaying();

        // Fires TogglePlayState unconditionally.
        bool TogglePlay();

        // Sends SetTrackSoloState for trackName with the given enabled flag.
        bool SetTrackSolo(const std::string& trackName, bool enabled);

        // Queries GetTrackList filtered to selected tracks and returns the first result's name.
        // Returns empty string on failure or if no track is selected.
        std::string GetSelectedTrackName();

        std::unique_ptr<PTSLC_CPP::CppPTSLClient> mClient;
        std::string mSoloedTrack;   // Name of the track currently soloed by EPPA
        bool        mIsConnected = false;
    };

} // namespace EPPA
