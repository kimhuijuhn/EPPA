#include "Reader.h"

#include <cstdlib>

// Delegates to the macOS `say` command so VoiceOver speaks the message.
// TODO: run on a background thread to avoid blocking MIDI callbacks.
void Reader::Speak(const std::string& /*message*/)
{
    // disabled until Reader is moved off the MIDI callback thread
}

void Reader::OnConnected()           { Speak("Connected to Pro Tools"); }
void Reader::OnConnectionFailed()    { Speak("Could not connect to Pro Tools"); }
void Reader::OnPlaybackStarted()     { Speak("Playing"); }
void Reader::OnPlaybackStopped()     { Speak("Stopped"); }
void Reader::OnTrackUnsoloed()       { Speak("Unsolo"); }
void Reader::OnNoTrackSelected()     { Speak("No track selected"); }

void Reader::OnTrackSoloed(const std::string& trackName)
{
    Speak(trackName);
}
