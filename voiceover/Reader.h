#pragma once

#include <string>

// Delivers audio feedback to the user via macOS VoiceOver (the `say` command).
// This is the "View" in the MVC — the only output channel for a VI user.
class Reader {
public:
    // Speaks an arbitrary message.
    void Speak(const std::string& message);

    // Domain-specific convenience methods.
    void OnConnected();
    void OnConnectionFailed();
    void OnPlaybackStarted();
    void OnPlaybackStopped();
    void OnTrackSoloed(const std::string& trackName);
    void OnTrackUnsoloed();
    void OnNoTrackSelected();
};
