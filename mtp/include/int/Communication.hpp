#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_COMMUNICATION_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_COMMUNICATION_HPP_

// headers from this package
#include "ext/PlayerId.hpp"
#include "int/PlayerPacket.hpp"
#include "int/AdapterRTDB.hpp"

// standard/system headers
#include <vector>

namespace mtp
{

class Communication
{
public:
    Communication(PlayerId const &id, bool path_encoding = false);
    ~Communication();

    std::vector<PlayerPacket> getPlayerPackets();
    void sendPlayerPacket(PlayerPacket const &packet);

private:
    PlayerId _id;
    AdapterRTDB _rtdb;
}; // end of class Communication

} // end of namespace mtp

#endif
