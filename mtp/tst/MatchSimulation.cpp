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

bool compareRobotClientPtr(RobotClient const *a, RobotClient const *b)
{
    if (a->id.teamId == b->id.teamId) return a->id.shirtId < b->id.shirtId;
    return a->id.teamId < b->id.teamId;
}

void MatchSimulation::addRobot(int vendorId, int shirtId, char teamId, float frequency, float jitter)
{
    _robots.push_back(new RobotClient(mtp::PlayerId(vendorId, shirtId, teamId), _t0, frequency, jitter));
    // sort robots by teamId, to improve readibility of report()
    sort(_robots.begin(), _robots.end(), compareRobotClientPtr);
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
            robot->tick(t);
        }
        report();
    }
}

bool MatchSimulation::ok() const
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
        if (!robot->readyToPlay()) // check 1
        {
            result = false;
            break;
        }
        roleCounts[robot->id.teamId][robot->getOwnRole()] += 1;
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

void MatchSimulation::report() const
{
    std::ostringstream ostr;
    for (const auto& robot: _robots)
    {
        printf("%s\n", robot->statusReport().c_str());
    }
}
