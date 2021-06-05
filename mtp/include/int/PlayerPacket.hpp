#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_PLAYERPACKET_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_PLAYERPACKET_HPP_

// standard/system headers
#include <vector>
#include <cstdint>

// other MSL packages
#include "RtDB2.h" // for serialization

namespace mtp
{

struct Ball
{
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    float vx = 0.0;
    float vy = 0.0;
    float vz = 0.0;
    float confidence = 0.0;

    SERIALIZE_DATA(x, y, z, vx, vy, vz, confidence);
}; // end of struct Ball

struct Obstacle
{
    float x = 0.0;
    float y = 0.0;
    float vx = 0.0;
    float vy = 0.0;
    float confidence = 0.0;

    SERIALIZE_DATA(x, y, vx, vy, confidence);
}; // end of struct Obstacle

struct PosVel
{
    float x = 0.0;
    float y = 0.0;
    float Rz = 0.0;
    float vx = 0.0;
    float vy = 0.0;
    float vRz = 0.0;
    float confidence = 0.0;

    SERIALIZE_DATA(x, y, Rz, vx, vy, vRz, confidence);
}; // end of struct PosVel

struct PlayerPacket
{
    uint8_t                vendor_id = 0;
    uint8_t                shirt_id = 0;
    char                   team_id = '?';
    int32_t                timestamp_ms = 0;
    uint8_t                has_ball = 0;
    std::vector<Ball>      balls;
    std::vector<Obstacle>  obstacles;
    std::vector<PosVel>    self_loc;
    uint8_t                role = 0;
    // TODO: inform others about desired role? optional<std::pair<uint8, float>> preference ?
    uint8_t                intention = 0;
    uint8_t                error = 0;

    SERIALIZE_DATA(vendor_id, shirt_id, team_id, timestamp_ms, has_ball, balls, obstacles, self_loc, role, intention, error);
}; // end of struct PlayerPacket

} // end of namespace mtp

#endif
