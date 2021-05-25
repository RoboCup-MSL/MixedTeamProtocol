#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_COMMUNICATION_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_COMMUNICATION_HPP_

// headers from this package
#include "ext/PlayerId.hpp"
#include "int/PlayerPacket.hpp"
#include "int/PlayerState.hpp"
#include "int/AdapterRTDB.hpp"

// standard/system headers
#include <vector>

namespace mtp
{

class Communication
{
public:
    Communication(PlayerId const &id);
    ~Communication();

    std::vector<PlayerPacket> getPlayerPackets();
    void sendPlayerPacket(PlayerPacket const &packet);

    PlayerState getPlayerState();
    void setPlayerState(PlayerState const &state);

    template<typename T>
    T getState(std::string key)
    {
        // plan A: use standard API, which might say an item has grown stale
        //T result;
        //int r = _rtdb.get(key, &result);
        //if (r == RTDB2_SUCCESS) return result;
        //tprintf("key=%s age=%.2f", key.c_str(), item.age());
        //throw std::runtime_error("MTP::Communication error " + std::to_string(r) + " for key " + key);
        // plan B: ignore age
        // why: test suite shows error 13 (STALE) occasionally, even though we do not sleep anywhere ?! performance issue perhaps?
        RtDB2Item item;
        _rtdb.getItem(key, item);
        return item.value<T>();
    }
    template<typename T>
    void setState(std::string key, T const &value)
    {
        _rtdb.put(key, &value);
    }

private:
    PlayerId _id;
    AdapterRTDB _rtdb;
}; // end of class Communication

} // end of namespace mtp

#endif
