#pragma once

#include <atomic>

#include "AppConfig.h"
#include "midi/MidiInputHandler.h"
#include "pedal/PedalInterpreter.h"
#include "pedal/PedalStateMachine.h"
#include "controller/ProToolsController.h"

namespace EPPA {

    // Top-level orchestrator. Owns all components, wires their interfaces together,
    // and drives the main event loop.
    class App {
    public:
        explicit App(AppConfig config);

        // Connects to Pro Tools, wires the MIDI → pedal → controller pipeline,
        // and opens the MIDI port. Returns false if any step fails.
        bool Start();

        // Blocks the calling thread with a 100ms polling loop until Shutdown() is called.
        // Call from main() after Start() returns true.
        void Run();

        // Signals the Run() loop to exit and performs teardown:
        // closes the MIDI port and unsolos any currently soloed track.
        void Shutdown();

    private:
        // Invoked by PedalStateMachine when the active zone changes.
        // Dispatches the appropriate Play / Stop / Solo / Unsolo actions.
        void OnZoneChange(PedalZone previous, PedalZone current);

        AppConfig           mConfig;
        MidiInputHandler    mMidi;
        PedalInterpreter    mInterpreter;
        PedalStateMachine   mStateMachine;
        ProToolsController  mController;
        std::atomic<bool>   mRunning{ false };
    };

} // namespace EPPA
