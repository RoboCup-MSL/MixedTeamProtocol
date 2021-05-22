#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_PLAYERID_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_PLAYERID_HPP_

// standard/system headers
#include <string>

namespace mtp
{

class PlayerId
{
public:
    PlayerId(int v, int s, char t = 'A');
    PlayerId(PlayerId const &other);

    int hash() const; // TODO?? uuid?
    std::string describe() const;
    bool valid() const;

    const int vendorId;
    const int shirtId;
    const char teamId;

}; // end of class PlayerId

bool operator<(PlayerId const &p1, PlayerId const &p2);
bool operator==(PlayerId const &p1, PlayerId const &p2);

} // end of namespace mtp

#endif
