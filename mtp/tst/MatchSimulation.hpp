#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_TST_MATCHSIMULATION_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_TST_MATCHSIMULATION_HPP_

// headers from this package
#include "RobotClient.hpp"

// standard/system headers
#include <vector>
#include <string>


// a simple simulation of a partial match, using multiple simulated robots
class MatchSimulation
{
    public:
        MatchSimulation();
        ~MatchSimulation();

    public:
        // test setup
        void addRobot(mtp::PlayerId const &playerId, float frequency = 10.0, float jitter = 0.0);
        void setPosVel(mtp::PlayerId const &playerId, mtp::Pose const &position, mtp::Pose const &velocity, float confidence);

        // runtime match simulation
        void advance(float duration = 1.0);
        // TODO: set refbox

        // state / result inspection
        bool ok() const;
        void report() const;

    private:
        std::vector<RobotClient*> _robots;
        std::vector<mtp::PlayerId> _players;
        std::map<mtp::PlayerId, mtp::Pose> _playerPose;
        rtime _t0;

        bool okRoleAllocation() const;
        bool okWorldModel() const;
        RobotClient* find(mtp::PlayerId const &playerId) const;
        std::vector<mtp::TeamMember> getTeam(mtp::PlayerId const &playerId) const;
};

#endif
