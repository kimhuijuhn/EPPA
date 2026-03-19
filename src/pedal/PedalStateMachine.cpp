#include "PedalStateMachine.h"

namespace EPPA {

void PedalStateMachine::SetOnZoneChange(ZoneChangeCallback callback)
{
    mOnZoneChange = std::move(callback);
}

void PedalStateMachine::Process(PedalZone newZone)
{
    // Dead bands are transparent — they do not change the active zone
    // and do not trigger any action.
    if (newZone == PedalZone::DEAD_LOW || newZone == PedalZone::DEAD_HIGH) return;

    // No transition if we are already in this zone.
    if (newZone == mCurrentZone) return;

    const PedalZone previous = mCurrentZone;
    mCurrentZone = newZone;

    if (mOnZoneChange) {
        mOnZoneChange(previous, mCurrentZone);
    }
}

PedalZone PedalStateMachine::GetCurrentZone() const
{
    return mCurrentZone;
}

} // namespace EPPA
