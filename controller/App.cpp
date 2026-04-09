#include "App.h"

#include <chrono>
#include <iostream>
#include <thread>

App::App()
    : mMidi(14)   // CC11 = standard expression pedal
{
}

bool App::Start()
{
    if (!mController.Connect()) {
        mReader.OnConnectionFailed();
        return false;
    }
    mReader.OnConnected();

    mMidi.SetCallback([this](int cc, int value) {
        OnMidiCC(cc, value);
    });

    if (!mMidi.OpenPort()) {
        std::cerr << "App: failed to open MIDI port\n";
        return false;
    }

    mRunning = true;
    return true;
}

void App::Run()
{
    while (mRunning)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void App::Shutdown()
{
    mController.UnsoloCurrentTrack();
    mMidi.Close();
    mRunning = false;
}

void App::OnMidiCC(int cc, int value)
{
    PedalZone newZone = mInterpreter.Interpret(value);
    std::cout << "CC" << cc << " value=" << value << "  zone=";
    switch (newZone) {
        case PedalZone::OFF:       std::cout << "OFF";       break;
        case PedalZone::DEAD_LOW:  std::cout << "DEAD_LOW";  break;
        case PedalZone::SOLO:      std::cout << "SOLO";      break;
        case PedalZone::DEAD_HIGH: std::cout << "DEAD_HIGH"; break;
        case PedalZone::ON:        std::cout << "ON";        break;
        default:                   std::cout << "UNKNOWN";   break;
    }
    std::cout << "\n";

    // Dead bands are transparent — ignore them to prevent boundary toggling.
    if (newZone == PedalZone::DEAD_LOW || newZone == PedalZone::DEAD_HIGH)
        return;

    if (newZone != mCurrentZone) {
        OnZoneChange(mCurrentZone, newZone);
        mCurrentZone = newZone;
    }
}

void App::OnZoneChange(PedalZone previous, PedalZone current)
{
    if (current == PedalZone::SOLO) {
        // stop→solo: solo + start playing
        // play→solo: solo + keep playing
        std::string trackName = mController.SoloSelectedTrack();
        if (!trackName.empty())
            mReader.OnTrackSoloed(trackName);
        else
            mReader.OnNoTrackSelected();

        if (previous == PedalZone::OFF)
            mController.Play();
    }
    else if (current == PedalZone::ON) {
        if (previous == PedalZone::SOLO) {
            // solo→play: already playing, just unsolo
            mController.UnsoloCurrentTrack();
        } else {
            // stop→play: start playing
            mController.Play();
        }
    }
    else if (current == PedalZone::OFF) {
        // solo→stop: unsolo + stop
        // play→stop: stop
        if (previous == PedalZone::SOLO)
            mController.UnsoloCurrentTrack();
        mController.Stop();
    }
}
