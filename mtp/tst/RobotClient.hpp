#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_TST_ROBOTCLIENT_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_TST_ROBOTCLIENT_HPP_

// headers from this package
#include "ext/MixedTeamProtocol.hpp"

// standard/system headers
// ...


// a simple simulation of a participating robot, relevant software only
// features/limitations:
// * this simple robot simulation can and will not move - such dynamical simulations are not really relevant for testing the protocol
// * robot will tell if it is ready to play (if not, then for instance it has not yet settled on a role)
// * vendor/shirt/team id cannot be reconfigured dynamically
class RobotClient
{
    public:
        RobotClient(mtp::PlayerId const &i, rtime const &t0, float frequency = 10.0, float jitter = 0.0);
        ~RobotClient();
        mtp::PlayerId id;

    public:
        // poke for an update at some frequency (10-40Hz) by controlling simulator
        void tick(rtime const &t);

        // role allocation and conflict resolution
        bool readyToPlay() const;
        std::string getOwnRole() const;

        // status report one liner
        std::string statusReport() const;

        // worldModel i/o

    private:
        mtp::MixedTeamProtocol _mtp;
        float _frequency;
        float _jitter;
};

#endif
