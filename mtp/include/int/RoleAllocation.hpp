#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_ROLEALLOCATION_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_ROLEALLOCATION_HPP_

// headers from this package
#include "ext/PlayerId.hpp"
#include "ext/Roles.hpp"

namespace mtp
{

typedef std::map<PlayerId, RoleEnum> RoleAllocation;
RoleCount roleAllocationToCount(RoleAllocation const &roles);

class RoleAllocationAlgorithm
{
public:
    // algorithm runs at construction and makes the results available
    // behavior:
    // * if role allocation is OK, then result == current and error == 0
    //   where 'OK' means:
    //     a) total allocation is according to the specification (min/max per role)
    //     b) if given, own role preference is satisfied (example: force becoming goalkeeper)
    // * otherwise, if role allocation is not ok:
    //   * calculate new allocation, minimize penalty score
    //     * things which contribute to penalty score: TBD
    //   * return result for own id (ignore others)
    //   * return a nonzero error to signal the current situation
    // other ideas:
    // * take into account all preferences / capabilities of other robots (requires more communication, at least)
    RoleAllocationAlgorithm(
        PlayerId const &myId, 
        RoleAllocation const &currentRoleAllocation, 
        RoleEnum const &myPreferredRole = RoleEnum::UNDEFINED,
        float myPreferredRoleFactor = 0.0
        );

    // algorithm result
    RoleAllocation result;
    uint8_t error = 0;
    std::string describe() const;

private:
    PlayerId _myId;
    RoleAllocation _currentRoleAllocation;
    RoleEnum _myPreferredRole = RoleEnum::UNDEFINED;
    float _myPreferredRoleFactor = 0.0;
    bool currentIsOk() const;
    void check() const;
    void run();
    std::vector<RoleAllocation> generateCandidates();
    float calculatePenalty(RoleAllocation const &candidate);

}; // end of class RoleAllocationAlgorithm

} // end of namespace mtp

#endif
