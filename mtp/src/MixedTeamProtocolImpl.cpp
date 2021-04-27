// header implemented in this file
#include "int/MixedTeamProtocolImpl.hpp"

// headers from this package
#include "ext/Roles.hpp"
#include "int/Errors.hpp"

// standard/system headers
#include <cmath>
#include <algorithm>


using namespace mtp;

MixedTeamProtocolImpl::MixedTeamProtocolImpl(PlayerId const &id)
:
    _id(id),
    _communication(std::make_shared<Communication>(id))
{
    _rng.seed(_id.hash());
}

MixedTeamProtocolImpl::~MixedTeamProtocolImpl()
{
}

bool MixedTeamProtocolImpl::good() const
{
    return _good;
}

std::string MixedTeamProtocolImpl::getOwnRole() const
{
    return roleEnumToString(_role);
}

std::vector<mtp::TeamMember> MixedTeamProtocolImpl::getTeam() const
{
    return std::vector<mtp::TeamMember>();
}

std::vector<mtp::Object> MixedTeamProtocolImpl::getBalls() const
{
    return std::vector<mtp::Object>();
}

std::vector<mtp::Object> MixedTeamProtocolImpl::getObstacles() const
{
    return std::vector<mtp::Object>();
}

void MixedTeamProtocolImpl::setOwnPosVel(mtp::Pose const &position, mtp::Pose const &velocity, float confidence)
{
}

void MixedTeamProtocolImpl::setOwnBalls(std::vector<mtp::Object> balls)
{
}

void MixedTeamProtocolImpl::setOwnObstacles(std::vector<mtp::Object> obstacles)
{
}

void MixedTeamProtocolImpl::setHumanTeamMember(mtp::Pose const &position, mtp::Pose const &velocity, float confidence)
{
}

void MixedTeamProtocolImpl::setOwnIntention(std::string intention)
{
}

void MixedTeamProtocolImpl::setPreferredOwnRole(std::string role, float preference)
{
}

void MixedTeamProtocolImpl::setT0(rtime const &t0)
{
    _t0 = t0;
}

void MixedTeamProtocolImpl::setCurrentTime(rtime const &t)
{
    _tc = t;
}

void MixedTeamProtocolImpl::start()
{
    _started = true;
}

void MixedTeamProtocolImpl::stop()
{
}

void MixedTeamProtocolImpl::tick(rtime const &t)
{
    // prepare
    _error = 0;
    // check if started
    //if (!_started) throw std::runtime_error("protocol violation: start() needs to be called first"); // ? TODO
    // check for new packets
    updatePlayers(_communication->getPlayerPackets());
    // worldModel processing (always, regardless of errors)
    calculateWorldModel(); // TODO: discuss: to which extent do we want to bring WM responsibility into here?
    // determine own role, if needed
    calculateOwnRole();
    // calculate _good and _error flags
    calculateGood();
    // send data
    _communication->sendPlayerPacket(makePacket());
}

void MixedTeamProtocolImpl::updatePlayers(std::vector<PlayerPacket> packets)
{
    // process new packets
    for (auto& packet: packets)
    {
        // determine PlayerId and ClientType hash
        PlayerId id(packet.vendor_id, packet.shirt_id, packet.team_id);
        ClientType c = id.hash();
        // ignore packet if from different team
        if (packet.team_id != _id.teamId) continue;
        // create if not yet existing
        if (!_players.count(c)) _players.insert(std::make_pair(c, Player(id)));
        // update Player object with packet
        _players.find(c)->second.update(packet);
    }
    // remove timed-out players
    float timeout = 7.0; // TODO make configurable
    for (auto it = _players.begin(); it != _players.end(); )
    {
        // calculate age in seconds, assume t0 synchronized
        rtime lastAlive = _t0 + 1e-3 * it->second.packet.timestamp_ms;
        bool isTimedOut = lastAlive - _tc > timeout;
        if (isTimedOut) it = _players.erase(it);
        else it++;
    }
}

void MixedTeamProtocolImpl::calculateWorldModel()
{
}

void MixedTeamProtocolImpl::calculateGood()
{
    _good = (_error == 0);
    // TODO: more?
}

RoleAllocation MixedTeamProtocolImpl::getCurrentRoleAllocation()
{
    RoleAllocation result;
    // count roles from team members
    for (auto const& player: _players)
    {
        result[player.second.id] = RoleEnum(player.second.packet.role);
    }
    // don't forget self
    result[_id] = RoleEnum(_role);
    return result;
}

void MixedTeamProtocolImpl::calculateOwnRole()
{
    // gather current role allocation
    auto currentRoles = getCurrentRoleAllocation();
    // run the algorithm
    RoleAllocationAlgorithm algo(_id, currentRoles, _preferredRoleString, _preferredRoleFactor);
    //std::cout << algo.describe() << std::endl; // DEBUG
    // handle result
    _error |= algo.error;
    _role = algo.result.at(_id);
}

PlayerPacket MixedTeamProtocolImpl::makePacket() const
{
    PlayerPacket result;
    result.vendor_id = _id.vendorId;
    result.shirt_id = _id.shirtId;
    result.team_id = _id.teamId;
    result.timestamp_ms = int(round(double(_tc - _t0) * 1000));
    // TODO: balls, obstacles, self_loc
    result.role = (uint8_t)_role;
    result.intention = _intention;
    result.error = _error;
    return result;
}
