// header implemented in this file
#include "RobotClient.hpp"

// standard/system headers
// ...


RobotClient::RobotClient(mtp::PlayerId const &i, rtime const &t0, float frequency, float jitter)
:
    id(i),
    _mtp(std::make_unique<mtp::MixedTeamProtocol>(id, true)),
    _comm(std::make_unique<mtp::Communication>(id, true)),
    _frequency(frequency),
    _jitter(jitter)
{
    (*_mtp)->setT0(t0);
}

RobotClient::~RobotClient()
{
    // no need to delete mtp, because it is a std::unique_ptr
}

void RobotClient::tick(rtime const &t)
{
    (*_mtp)->setCurrentTime(t);
    (*_mtp)->tick(t);
}

bool RobotClient::readyToPlay() const
{
    return (*_mtp)->good();
}

mtp::RoleEnum RobotClient::getOwnRole() const
{
    return (*_mtp)->getOwnRole();
}

void RobotClient::setCurrentRole(mtp::RoleEnum const &role)
{
    _comm->setState("CURRENT_ROLE", (int)role);
}

void RobotClient::setPreferredRole(mtp::RoleEnum const &role)
{
    mtp::PreferredRole pr;
    pr.role = (int)role;
    pr.preference = 1.0;
    _comm->setState("PREFERRED_ROLE", pr);
}

void RobotClient::setOwnPosVel(mtp::Pose const &position, mtp::Pose const &velocity, float confidence)
{
    _comm->setState("OWN_POS_VEL", mtp::PosVel(position, velocity, confidence));
}

std::vector<mtp::TeamMember> RobotClient::getTeam() const
{
    return (*_mtp)->getTeam();
}

const char *bool2str(bool b)
{
    return b ? "true" : "false";
}

std::string RobotClient::statusReport() const
{
    char buf[80] = {0};
    sprintf(buf, "%s ready=%5s role=%s", id.describe().c_str(), bool2str(readyToPlay()), mtp::roleEnumToString(getOwnRole()).c_str());
    return std::string(buf);
}
