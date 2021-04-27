#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_ERRORS_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_ERRORS_HPP_

namespace mtp
{

// use single bits so errors can be combined 
const int ERROR_BAD_ROLE_ELSEWHERE = 1;
const int ERROR_BAD_ROLE_SELF = 2;
const int ERROR_SHIRT_CONFLICT = 4;

} // end of namespace mtp

#endif
