// header implemented in this file
#include "MatchSimulation.hpp"

// header from MTP
#include "Roles.hpp"

// standard/system headers
// ...


MatchSimulation::MatchSimulation()
:
    _t0(rtime::now())
{
}

MatchSimulation::~MatchSimulation()
{
}

bool operator<(RobotClient const &a, RobotClient const &b)
{
    if (a.id.teamId == b.id.teamId) return a.id.shirtId < b.id.shirtId;
    return a.id.teamId < b.id.teamId;
}

RobotClient &MatchSimulation::addRobot(mtp::PlayerId const &playerId, float frequency, float jitter)
{
    if (_robots.count(playerId)) throw std::runtime_error("player already registered: " + playerId.describe());
    _robots.try_emplace(playerId, RobotClient(playerId, _t0, frequency, jitter));
    return getRobot(playerId);
}

void MatchSimulation::setPosVel(mtp::PlayerId const &playerId, mtp::Pose const &position, mtp::Pose const &velocity, float confidence)
{
    // store for comparison later
    _playerPose.insert(std::pair<mtp::PlayerId, mtp::Pose>(playerId, position));

    RobotClient &rc = getRobot(playerId);
    rc.setOwnPosVel(position, velocity, confidence);
}

void MatchSimulation::advance(float duration)
{
    rtime t = _t0;
    rtime te = _t0 + duration;
    float dt = 0.1;
    while (t < te)
    {
        t += dt;
        printf("\n");
        printf("simulation step START timestamp: %s\n", t.toStr().c_str());
        // poke robots
        for (auto& robot: _robots)
        {
            robot.second.tick(t);
        }
        report();
    }
}

bool MatchSimulation::ok() const
{
    return okRoleAllocation() && okWorldModel();
}

bool MatchSimulation::okRoleAllocation() const
{
    // checks:
    // 1. each robot must report ready-to-play
    // 2. the role allocation per team must be valid
    // notes:
    // * it can happen that check 1 is true, but 2 not, in case robots are not communicating with each other
    bool result = true;
    std::map<char, mtp::RoleCount> roleCounts;
    for (const auto& robot: _robots)
    {
        if (!robot.second.readyToPlay()) // check 1
        {
            result = false;
            break;
        }
        roleCounts[robot.second.id.teamId][robot.second.getOwnRole()] += 1;
    }
    // check 2
    if (roleCounts.size() < 1) result = false;
    if (roleCounts.size() > 2) result = false;
    for (const auto& count: roleCounts)
    {
        if (!mtp::checkRoleCount(count.second)) result = false;
    }
    return result;
}

bool MatchSimulation::okWorldModel() const
{
    bool result = true;
    /*for (auto &p : _players)
    {
        result = result && (getTeam(p).size() == 5);
        for (auto &member : getTeam(p))
        {
            result = result && (member.position.x == _playerPose.find(member.id)->second.x);
            result = result && (member.position.y == _playerPose.find(member.id)->second.y);
        }
    }*/
    return result;
}

void MatchSimulation::report() const
{
    std::ostringstream ostr;
    for (const auto& robot: _robots)
    {
        printf("%s\n", robot.second.statusReport().c_str());
    }
}

RobotClient& MatchSimulation::getRobot(mtp::PlayerId const &playerId)
{
    try
    {
        return _robots.at(playerId);
    }
    catch (...)
    {
        throw std::runtime_error("player not registered: " + playerId.describe());
    }
}

std::vector<mtp::TeamMember> MatchSimulation::getTeam(mtp::PlayerId const &playerId)
{
    RobotClient &rc = getRobot(playerId);
    return rc.getTeam();
}
