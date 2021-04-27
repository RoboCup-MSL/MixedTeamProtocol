#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_TEAMMEMBER_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_TEAMMEMBER_HPP_

// standard/system headers
#include <string>

// headers from this package
#include "PlayerId.hpp"
#include "Pose.hpp"

namespace mtp
{

struct TeamMember
{
    PlayerId id;
    std::string role;
    std::string intention;
    Pose position;
    Pose velocity;
}; // end of class TeamMember

} // end of namespace mtp

#endif
