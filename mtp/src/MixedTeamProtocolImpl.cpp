// header implemented in this file
#include "int/MixedTeamProtocolImpl.hpp"

// headers from this package
#include "ext/Roles.hpp"
#include "int/Errors.hpp"

// standard/system headers
#include <cmath>
#include <algorithm>


using namespace mtp;

MixedTeamProtocolImpl::MixedTeamProtocolImpl(PlayerId const &id, bool path_encoding)
:
    _id(id),
    _communication(std::make_shared<Communication>(id, path_encoding))
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

RoleEnum MixedTeamProtocolImpl::getOwnRole() const
{
    return (RoleEnum)_communication->getState<int>("CURRENT_ROLE");
}

std::vector<mtp::TeamMember> MixedTeamProtocolImpl::getTeam() const
{
    std::vector<mtp::TeamMember> result;
    for (auto const &player: _players)
    {
        mtp::TeamMember t(player.second.id);
        t.role = roleEnumToString((mtp::RoleEnum)player.second.packet.role);
        if (!player.second.packet.self_loc.empty()) {
            auto const &pv = player.second.packet.self_loc.front();
            t.position.x = pv.x;
            t.position.y = pv.y;
            t.position.rz = pv.rz;
            t.velocity.x = pv.vx;
            t.velocity.y = pv.vy;
            t.velocity.rz = pv.vrz;
            // TODO: what to do with confidence?
        }
        // TODO: set t.intention
        result.push_back(t);
    }
    return result;
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
    mtp::PosVel pv;
    pv.x = position.x;
    pv.y = position.y;
    pv.rz = position.rz;
    pv.vx = velocity.x;
    pv.vy = velocity.y;
    pv.vrz = velocity.rz;
    pv.confidence = confidence;
    _communication->setState("OWN_POS_VEL", pv);
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

void MixedTeamProtocolImpl::setPreferredOwnRole(RoleEnum const &role, float preference)
{
    mtp::PreferredRole r;
    r.role = (int)role;
    r.preference = preference;
    _communication->setState("PREFERRED_ROLE", r);
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
    // check for new packets, get _state struct
    _state = _communication->getPlayerState();
    auto k = _communication->getPlayerPackets();
    updatePlayers(k);
    // worldModel processing (always, regardless of errors)
    calculateWorldModel(); // TODO: discuss: to which extent do we want to bring WM responsibility into here?
    // determine own role, if needed
    calculateOwnRole();
    // calculate _good and _error flags
    calculateGood();
    // send data packet, write _state struct
    _communication->setPlayerState(_state);
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
    result[_id] = getOwnRole();
    return result;
}

void MixedTeamProtocolImpl::calculateOwnRole()
{
    // gather current role allocation
    auto currentRoles = getCurrentRoleAllocation();
    // run the algorithm
    RoleAllocationAlgorithmBruteForce algo(_id, currentRoles, (mtp::RoleEnum)_state.preferredRole.role, _state.preferredRole.preference);
    algo.run();
    tprintf("algorithm result:\n%s", algo.describe().c_str()); // DEBUG
    // handle result
    _error |= algo.error;
    _state.currentRole = (int)algo.result.at(_id);
}

PlayerPacket MixedTeamProtocolImpl::makePacket() const
{
    PlayerPacket result;
    result.vendor_id = _id.vendorId;
    result.shirt_id = _id.shirtId;
    result.team_id = _id.teamId;
    result.timestamp_ms = int(round(double(_tc - _t0) * 1000));
    result.self_loc.push_back(_state.ownPosVel);
    // TODO: balls, obstacles
    result.role = (uint8_t)getOwnRole();
    result.intention = _state.intention;
    result.error = _error;
    return result;
}
