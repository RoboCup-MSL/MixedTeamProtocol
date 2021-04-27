#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_ADAPTERRTDB_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_ADAPTERRTDB_HPP_

// headers from external dependencies - for now, we use the Falcons "rtdb2.5"
#include "RtDB2.h"
typedef int ClientType; // TODO: Rob is considering to use uuid's, moving away from traditional int agent id and all associated limitations
// (however we should take care that this doesn't make things more difficult to diagnose (?))

// standard/system headers
#include <vector>

namespace mtp
{

class AdapterRTDB: public RtDB2
{
public:
    AdapterRTDB(ClientType id);
    ~AdapterRTDB();

    std::set<ClientType> getClients(); // TODO const; - require some fundamental rework in RTDB --> v3?

private:

}; // end of class AdapterRTDB

} // end of namespace mtp

#endif
