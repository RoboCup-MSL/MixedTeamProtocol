#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_ADAPTERRTDB_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_ADAPTERRTDB_HPP_

// headers from external dependencies
#include "RtDB2.h"
#include "Player.hpp"

typedef int ClientType; // TODO: Rob is considering to use uuid's, moving away from traditional int agent id and all associated limitations
// (however we should take care that this doesn't make things more difficult to diagnose (?))

#define MTP_RTDB_STORAGE_PATH "/tmp/rtdb_"

// standard/system headers
#include <vector>

namespace mtp
{

class AdapterRTDB: public RtDB2
{
public:
    AdapterRTDB(PlayerId const &id, std::string const &dbname = "default", bool path_encoding = false);
    ~AdapterRTDB();

    std::set<ClientType> getClients(); // TODO const; - require some fundamental rework in RTDB --> v3?

private:
    RtDB2Context createContext(PlayerId const &id, std::string const &dbname, bool path_encoding);

}; // end of class AdapterRTDB

} // end of namespace mtp

#endif
