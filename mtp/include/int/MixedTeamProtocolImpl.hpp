#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_IMPLEMENTATION_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_IMPLEMENTATION_HPP_

// headers from this package
#include "ext/MixedTeamProtocol.hpp"
#include "ext/PlayerId.hpp"
#include "ext/Roles.hpp"
#include "int/PlayerPacket.hpp"
#include "int/Player.hpp"
#include "int/Communication.hpp"
#include "int/RoleAllocation.hpp"

// standard/system headers
#include <random>

namespace mtp
{

class MixedTeamProtocolImpl: public Interface
{
public:
    MixedTeamProtocolImpl(PlayerId const &id);
    ~MixedTeamProtocolImpl();

    // implementation of the API
    bool good() const;
    std::string getOwnRole() const;
    std::vector<mtp::TeamMember> getTeam() const;
    std::vector<mtp::Object> getBalls() const;
    std::vector<mtp::Object> getObstacles() const;
    void setOwnPosVel(mtp::Pose const &position, mtp::Pose const &velocity, float confidence);
    void setOwnBalls(std::vector<mtp::Object> balls);
    void setOwnObstacles(std::vector<mtp::Object> obstacles);
    void setHumanTeamMember(mtp::Pose const &position, mtp::Pose const &velocity, float confidence);
    void setOwnIntention(std::string intention);
    void setPreferredOwnRole(std::string role, float preference);
    void setT0(rtime const &t0);
    void setCurrentTime(rtime const &t);
    void start();
    void stop();
    void tick(rtime const &t);

private:
    // data members
    bool _good = false;
    PlayerId _id;
    bool _started = false;
    rtime _t0, _tc;
    RoleEnum _role = RoleEnum::UNDEFINED;
    uint8_t _intention;
    uint8_t _error;
    std::default_random_engine _rng;
    std::string _preferredRoleString = "UNDEFINED";
    float _preferredRoleFactor = 0.0;
    std::shared_ptr<Communication> _communication;
    std::map<ClientType, Player> _players;

    // functions
    void updatePlayers(std::vector<PlayerPacket> packets);
    void calculateWorldModel();
    void calculateGood();
    RoleAllocation getCurrentRoleAllocation();
    void calculateOwnRole();
    PlayerPacket makePacket() const;

}; // end of class MixedTeamProtocolImpl

} // end of namespace mtp


#endif
