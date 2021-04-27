// header implemented in this file
#include "RobotClient.hpp"

// standard/system headers
// ...


RobotClient::RobotClient(mtp::PlayerId const &i, rtime const &t0, float frequency, float jitter)
:
    id(i),
    _mtp(i),
    _frequency(frequency),
    _jitter(jitter)
{
    _mtp->setT0(t0);
}

RobotClient::~RobotClient()
{
}

void RobotClient::tick(rtime const &t)
{
    _mtp->setCurrentTime(t);
    _mtp->tick(t);
}

bool RobotClient::readyToPlay() const
{
    return _mtp->good();
}

std::string RobotClient::getOwnRole() const
{
    return _mtp->getOwnRole();
}

const char *bool2str(bool b)
{
    return b ? "true" : "false";
}

std::string RobotClient::statusReport() const
{
    char buf[80] = {0};
    sprintf(buf, "%s ready=%5s role=%s", id.describe().c_str(), bool2str(readyToPlay()), getOwnRole().c_str());
    return std::string(buf);
}
