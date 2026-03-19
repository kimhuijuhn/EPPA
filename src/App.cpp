#include "App.h"

#include <chrono>
#include <iostream>
#include <thread>

namespace EPPA {

App::App(AppConfig config)
    : mConfig(std::move(config))
    , mMidi(mConfig.midiCC, mConfig.midiChannel)
    , mInterpreter(mConfig)
    , mController(mConfig)
{}

bool App::Start()
{
    // 1. Establish connection to Pro Tools.
    if (!mController.Connect()) {
        std::cerr << "[EPPA] Failed to connect to Pro Tools. Is it running?\n";
        return false;
    }

    // 2. Register zone-change handler so Pro Tools actions fire on transitions.
    mStateMachine.SetOnZoneChange([this](PedalZone prev, PedalZone curr) {
        OnZoneChange(prev, curr);
    });

    // 3. Wire raw MIDI messages through the interpreter and into the state machine.
    //    This callback fires on RtMidi's internal thread.
    mMidi.SetCallback([this](const MidiMessage& msg) {
        const PedalZone zone = mInterpreter.Interpret(msg.value);
        mStateMachine.Process(zone);
    });

    // 4. Show available ports and open the configured one.
    const auto ports = mMidi.GetPortNames();
    if (ports.empty()) {
        std::cerr << "[EPPA] No MIDI input ports available.\n";
        return false;
    }

    std::cout << "[EPPA] Available MIDI ports:\n";
    for (const auto& name : ports) {
        std::cout << "  - " << name << "\n";
    }

    if (!mMidi.OpenPort(mConfig.midiPortName)) {
        std::cerr << "[EPPA] Could not open MIDI port.\n";
        return false;
    }

    mRunning = true;
    std::cout << "[EPPA] Listening for pedal input. Press Ctrl+C to quit.\n";
    return true;
}

void App::Run()
{
    while (mRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void App::Shutdown()
{
    if (!mRunning) return;
    mRunning = false;

    mMidi.Close();
    mController.UnsoloCurrentTrack();  // leave Pro Tools in a clean state

    std::cout << "[EPPA] Shutdown complete.\n";
}

void App::OnZoneChange(PedalZone previous, PedalZone current)
{
    switch (current) {
        case PedalZone::ON:
            // Arriving from SOLO: unsolo the track before starting playback.
            if (previous == PedalZone::SOLO) {
                mController.UnsoloCurrentTrack();
            }
            mController.Play();
            break;

        case PedalZone::SOLO:
            // Solo the currently selected track regardless of arrival direction.
            // SoloSelectedTrack() handles cleaning up any previously soloed track.
            mController.SoloSelectedTrack();
            break;

        case PedalZone::OFF:
            // Arriving from SOLO: unsolo the track before stopping playback.
            if (previous == PedalZone::SOLO) {
                mController.UnsoloCurrentTrack();
            }
            mController.Stop();
            break;

        default:
            break;
    }
}

} // namespace EPPA
