#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_PLAYERSTATE_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_PLAYERSTATE_HPP_

// standard/system headers
#include <vector>

// headers from this package
#include "Roles.hpp"
#include "PlayerPacket.hpp"

// other MSL packages
#include "RtDB2.h" // for serialization

namespace mtp
{

struct PreferredRole
{
    int role = (int)RoleEnum::UNDEFINED;
    float preference = 1.0;

    SERIALIZE_DATA(role, preference);
}; // end of struct PreferredRole

struct PlayerState
{
    int currentRole = (int)RoleEnum::UNDEFINED;
    PreferredRole preferredRole;
    int intention = 0;
    PosVel ownPosVel;

    SERIALIZE_DATA(currentRole, preferredRole, intention, ownPosVel);
}; // end of struct PlayerState

} // end of namespace mtp

#endif
