#include <csignal>
#include <iostream>

#include "App.h"

// Global pointer used by the signal handler to trigger a clean shutdown.
static EPPA::App* gApp = nullptr;

static void SignalHandler(int /*signal*/)
{
    std::cout << "\n[EPPA] Signal received, shutting down...\n";
    if (gApp) {
        gApp->Shutdown();
    }
}

int main()
{
    // --- Configuration ---
    // Adjust these values to match your expression pedal and MIDI setup.
    EPPA::AppConfig config;
    // config.midiCC        = 11;              // CC11 is standard for expression pedals
    // config.midiChannel   = -1;              // -1 = accept any channel
    // config.midiPortName  = "MyPedalName";   // empty = open first available port

    // --- Build and start the app ---
    EPPA::App app(config);
    gApp = &app;

    std::signal(SIGINT,  SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    if (!app.Start()) {
        return 1;
    }

    app.Run();
    return 0;
}
