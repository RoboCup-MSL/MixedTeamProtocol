#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_PLAYERSTATE_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_PLAYERSTATE_HPP_

// the PlayerState struct is written into RTDB so it can be logged and possibly modified by test suite
// (before, there were a bunch of data members in MixedTeamProtocolImpl.hpp)


// standard/system headers
#include <vector>

// headers from this package
#include "Roles.hpp"
#include "PlayerPacket.hpp"

// other MSL packages
#include "RtDB2.h" // for serialization

namespace mtp
{

struct PlayerState
{
    int currentRole = (int)RoleEnum::UNDEFINED;
    int preferredRole = (int)RoleEnum::UNDEFINED;
    float preferenceFactor = 1.0;
    int intention = 0;
    PosVel ownPosVel;
    bool isLeader = false;
    bool hasBall = false;

    SERIALIZE_DATA(currentRole, preferredRole, preferenceFactor, intention, ownPosVel, isLeader, hasBall);
}; // end of struct PlayerState

} // end of namespace mtp

#endif
