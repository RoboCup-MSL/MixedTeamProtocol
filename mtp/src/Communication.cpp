// header implemented in this file
#include "int/Communication.hpp"

using namespace mtp;


Communication::Communication(PlayerId const &id, bool path_encoding)
:
    _id(id),
    _rtdb(id, "mixedteam", path_encoding),
    _rtdbRefbox(id, "refbox", path_encoding)
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

        int r = _rtdb.get("MTP", &packet, client); // too slow?!!!
        if (r == RTDB2_SUCCESS) // TODO: revise RTDB API in v3 to not return magic int values, instead, apply exception handling
        {
            // same-team checks and timeout checks are done in Player packet handler
            result.push_back(packet);
        }
        else
        {
            if (r == RTDB2_ITEM_STALE)
            {
                RtDB2Item item;
                _rtdb.getItem("MTP", item);
                tprintf("WARNING: timeout (age %.2fs): could not read MTP packet for client %d at %s", item.age(), client, _id.describe().c_str());
            }
            // if only RTDB would just throw clear exceptions. TODO
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
    _rtdb.get("HAS_BALL", &result.hasBall);
    return result;
}

void Communication::setPlayerState(PlayerState const &state)
{
    _rtdb.put("CURRENT_ROLE", &state.currentRole);
    _rtdb.put("PREFERRED_ROLE", &state.preferredRole);
    _rtdb.put("INTENTION", &state.intention);
    _rtdb.put("OWN_POS_VEL", &state.ownPosVel);
    _rtdb.put("HAS_BALL", &state.hasBall);
}

RefereeCommand Communication::getLastCommand()
{
    std::string command;
    std::string target;
    bool success = (_rtdbRefbox.get("COMMAND", &command, 0) == RTDB2_SUCCESS) &&
                   (_rtdbRefbox.get("TARGETTEAM", &target, 0) == RTDB2_SUCCESS);
    RefereeCommand result;
    if (success)
    {
        result.command = commandStringToEnum(command);
        result.target = targetStringToEnum(target);
    }
    return result;
}
