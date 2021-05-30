// header implemented in this file
#include "int/AdapterRTDB.hpp"

// standard/system headers
// ...

using namespace mtp;

AdapterRTDB::AdapterRTDB(PlayerId const &id, std::string const &dbname, bool path_encoding)
:
    RtDB2(createContext(id, dbname, path_encoding))
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

RtDB2Context AdapterRTDB::createContext(PlayerId const &id, std::string const &dbname, bool path_encoding)
{
    if (path_encoding)
    {
        return RtDB2Context::Builder(id.shirtId)
            .withoutConfigFile()
            .withRootPath(MTP_RTDB_STORAGE_PATH +
                std::string(dbname) +
                std::string("_") +
                std::string(1, id.teamId)).build();
    }
    return RtDB2Context::Builder(id.shirtId)
            .withoutConfigFile()
            .withDatabase(dbname).build();
}