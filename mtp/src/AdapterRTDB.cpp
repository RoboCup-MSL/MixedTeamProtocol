// header implemented in this file
#include "int/AdapterRTDB.hpp"

// standard/system headers
// ...

using namespace mtp;

AdapterRTDB::AdapterRTDB(ClientType id, char teamId)
:
    RtDB2(RtDB2Context::Builder(id).withoutConfigFile().withRootPath(MTP_RTDB_STORAGE_PATH + std::string("_") + std::string(1, teamId)).build())
{
    auto const &c = getContext();
    std::ostringstream os;
    os << c;
    tprintf("RTDB configuration:\n%s", os.str().c_str()); // TODO: put this behind a MTP_VERBOSE compilation flag?
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
