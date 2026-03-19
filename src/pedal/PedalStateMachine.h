#pragma once

#include <functional>

#include "PedalZone.h"

namespace EPPA {

    using ZoneChangeCallback = std::function<void(PedalZone previous, PedalZone current)>;

    // Tracks the active pedal zone and fires a callback only when the zone
    // meaningfully changes. Dead band zones (DEAD_LOW, DEAD_HIGH) are transparent:
    // they do not update the active zone and do not trigger the callback.
    // This prevents rapid toggling when the pedal hovers near a zone boundary.
    class PedalStateMachine {
    public:
        PedalStateMachine() = default;

        // Registers the callback invoked on every active-zone transition.
        // Called with (previousZone, newZone).
        void SetOnZoneChange(ZoneChangeCallback callback);

        // Feeds a newly interpreted zone into the state machine.
        // DEAD_LOW and DEAD_HIGH are silently discarded.
        // All other zones update the active zone if they differ from the current one.
        void Process(PedalZone newZone);

        PedalZone GetCurrentZone() const;

    private:
        PedalZone         mCurrentZone = PedalZone::UNKNOWN;
        ZoneChangeCallback mOnZoneChange;
    };

} // namespace EPPA
