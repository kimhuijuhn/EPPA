#pragma once

#include <atomic>

#include "MidiInputHandler.h"
#include "PedalInterpreter.h"
#include "PlaybackController.h"
#include "Reader.h"

// Top-level coordinator. Owns all components, wires them together,
// and drives the main event loop.
class App {
public:
    App();

    // Connects to Pro Tools, opens the MIDI port, and wires the
    // MIDI → interpreter → controller → reader pipeline.
    // Returns false if any step fails.
    bool Start();

    // Blocks until Shutdown() is called (100ms polling loop).
    void Run();

    // Signals Run() to exit and performs teardown.
    void Shutdown();

private:
    // Invoked by MidiInputHandler on each matching CC message.
    void OnMidiCC(int cc, int value);

    // Invoked when the pedal moves to a new active zone.
    void OnZoneChange(PedalZone previous, PedalZone current);

    MidiInputHandler   mMidi;
    PedalInterpreter   mInterpreter;
    PlaybackController mController;
    Reader             mReader;

    PedalZone          mCurrentZone = PedalZone::UNKNOWN;
    std::atomic<bool>  mRunning{ false };
};
