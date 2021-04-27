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
        void addRobot(int vendorId, int shirtId, char teamId, float frequency = 10.0, float jitter = 0.0);

        // runtime match simulation
        void advance(float duration = 1.0);
        // TODO: set refbox

        // state / result inspection
        bool ok() const;
        void report() const;

    private:
        std::vector<RobotClient*> _robots;
        rtime _t0;
};

#endif
