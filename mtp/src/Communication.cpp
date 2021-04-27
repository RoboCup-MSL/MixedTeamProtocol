// header implemented in this file
#include "int/Communication.hpp"

using namespace mtp;


Communication::Communication(PlayerId const &id)
:
    _id(id),
    _rtdb(_id.hash())
{
    // TODO: how to ensure current id is not already claimed? Rob? Should make requirement + test case in RTDB layer.
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
