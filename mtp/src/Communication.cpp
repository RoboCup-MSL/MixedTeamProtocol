// header implemented in this file
#include "int/Communication.hpp"

using namespace mtp;


Communication::Communication(PlayerId const &id, bool path_encoding)
:
    _id(id),
    _rtdb(id, "mixedteam", path_encoding)
{
    // TODO: how to ensure current id is not already claimed? Rob? Should make requirement + test case in RTDB layer.

    // initialize RTDB state
    setPlayerState(PlayerState());
}

Communication::~Communication()
{
}

std::vector<PlayerPacket> Communication::getPlayerPackets()
{
    std::vector<PlayerPacket> result;
    auto clients = _rtdb.getClients();
    for (auto& client: clients)
    {
        PlayerPacket packet;
        int r = _rtdb.get("MTP", &packet, client);
        if (r == RTDB2_SUCCESS) // TODO: revise RTDB API in v3 to not return magic int values, instead, apply exception handling
        {
            // same-team checks and timeout checks are done in Player packet handler
            result.push_back(packet);
        }
    }
    return result;
}

void Communication::sendPlayerPacket(PlayerPacket const &packet)
{
    _rtdb.put("MTP", &packet);
}

PlayerState Communication::getPlayerState()
{
    PlayerState result;
    _rtdb.get<int>("CURRENT_ROLE", &result.currentRole);
    _rtdb.get("PREFERRED_ROLE", &result.preferredRole);
    _rtdb.get("INTENTION", &result.intention);
    _rtdb.get("OWN_POS_VEL", &result.ownPosVel);
    return result;
}

void Communication::setPlayerState(PlayerState const &state)
{
    _rtdb.put("CURRENT_ROLE", &state.currentRole);
    _rtdb.put("PREFERRED_ROLE", &state.preferredRole);
    _rtdb.put("INTENTION", &state.intention);
    _rtdb.put("OWN_POS_VEL", &state.ownPosVel);
}
