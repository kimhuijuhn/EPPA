#include "CppPTSLClient.h"

#include "host_ready_check.h"
#include "register_connection.h"
#include "get_session_name.h"
#include "get_track_list.h"
#include "toggle_play_state.h"

using namespace PTSLC_CPP;

int main()
{
    ClientConfig config;
    config.address        = "localhost:31416";
    config.serverMode     = Mode::Mode_ProTools;
    config.skipHostLaunch = SkipHostLaunch::SHLaunch_No;

    CppPTSLClient client(config);

    DoHostReadyCheck(client);
    DoRegisterConnection(client);
    DoGetSessionName(client);
    DoGetTrackList(client);
    // DoTogglePlayState(client); // uncomment to toggle playback

    return 0;
}
