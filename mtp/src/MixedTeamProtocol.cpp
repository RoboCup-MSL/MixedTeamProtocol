// header implemented in this file
#include "ext/MixedTeamProtocol.hpp"

// other headers from this package
#include "int/MixedTeamProtocolImpl.hpp"


using namespace mtp;

MixedTeamProtocol::MixedTeamProtocol(PlayerId const &id)
    : impl(std::make_unique<MixedTeamProtocolImpl>(id))
{
}
