// header implemented in this file
#include "int/AdapterRTDB.hpp"

// standard/system headers
// ...

using namespace mtp;

AdapterRTDB::AdapterRTDB(ClientType id)
:
    RtDB2(RtDB2Context::Builder(id).withoutConfigFile().withRootPath(MTP_RTDB_STORAGE_PATH).build())
{
}

AdapterRTDB::~AdapterRTDB()
{
}

std::set<ClientType> AdapterRTDB::getClients() // TODO: const
{
    std::set<ClientType> result;
    // TODO: solve this properly in RTDB3 ... see also hardcoded agent limit 10 in RtDB2.cpp
    RtDB2FrameSelection selection;
    selection.local = true;
    selection.shared = true;
    RtDB2Frame frame;
    if (getFrame(frame, selection) == RTDB2_SUCCESS)
    {
        for (auto item: frame.items)
        {
            result.insert(ClientType(item.agent));
        }
    }
    return result;
}
