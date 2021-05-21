// header implemented in this file
#include "int/RoleAllocation.hpp"

// headers from this package
#include "int/Errors.hpp"

// system headers
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace mtp;


RoleCount mtp::roleAllocationToCount(RoleAllocation const &roles)
{
    RoleCount result;
    for (auto const& rolePair: roles)
    {
        result[rolePair.second] += 1;
    }
    return result;
}

RoleAllocationAlgorithm::RoleAllocationAlgorithm(
    PlayerId const &myId,
    RoleAllocation const &currentRoleAllocation,
    RoleEnum const &myPreferredRole,
    float myPreferredRoleFactor
    )
:
    _myId(myId),
    _currentRoleAllocation(currentRoleAllocation),
    _myPreferredRole(myPreferredRole),
    _myPreferredRoleFactor(myPreferredRoleFactor)
{
    // input checks
    check();
    // initialize output
    result.clear();
    error = 0;
    // run
    run();
}

void RoleAllocationAlgorithm::check() const
{
    if (!_myId.valid())
    {
        throw std::runtime_error("RoleAllocationAlgorithm got an invalid player id (self): " + _myId.describe());
    }
    if (_myPreferredRoleFactor < 0.0 || _myPreferredRoleFactor > 1.0)
    {
        throw std::runtime_error("RoleAllocationAlgorithm got an invalid role preference factor: " + std::to_string(_myPreferredRoleFactor));
    }
    for (auto const& imap: _currentRoleAllocation)
    {
        if (!imap.first.valid())
        {
            throw std::runtime_error("RoleAllocationAlgorithm got an invalid player id (current roles): " + imap.first.describe());
        }
        try
        {
            std::string r = roleEnumToString(imap.second);
        }
        catch (...)
        {
            throw std::runtime_error("RoleAllocationAlgorithm got an invalid current role");
        }
    }
}

bool RoleAllocationAlgorithm::currentIsOk() const
{
    // check own role preference
    if (_myPreferredRoleFactor > 0.0)
    {
        if (_currentRoleAllocation.at(_myId) != _myPreferredRole) return false;
    }
    // check role count against specification
    auto count = roleAllocationToCount(_currentRoleAllocation);
    return checkRoleCount(count);
}

void RoleAllocationAlgorithm::run()
{
    // check if current situation is OK
    if (currentIsOk())
    {
        result = _currentRoleAllocation;
        return;
    }
    // generate candidates
    auto candidates = generateCandidates(); // TODO caching?
    // pick the best one
    float bestPenalty = 1e9;
    for (auto candidate: candidates)
    {
        float penalty = calculatePenalty(candidate);
        if (penalty < bestPenalty)
        {
            bestPenalty = penalty;
            result = candidate;
        }
    }
    //printf("bestPenalty=%e\n", bestPenalty); //DEBUG
    // TODO random pick?
    // TODO determine error
}

std::string RoleAllocationAlgorithm::describe() const
{
    std::ostringstream ostr;
    // print algorithm result versus input
    ostr << "Result code: " << (int)error << std::endl;
    ostr << "Result allocation:" << std::endl;
    for (auto const& rolePair: result)
    {
        std::string selfString = "      ";
        if (rolePair.first == _myId) selfString = "[self]";
        ostr << "  " << selfString << " " << rolePair.first.describe() << ": " << std::setw(20) << std::left;
        try
        {
            ostr << mtp::roleEnumToString(rolePair.second);
        }
        catch (...)
        {
            ostr << "ERROR(" << (int)rolePair.second << ")";
        }
        std::string beforeString = "";
        if (_currentRoleAllocation.count(rolePair.first))
        {
            beforeString = "(" + mtp::roleEnumToString(_currentRoleAllocation.at(rolePair.first)) + ")";
        }
        ostr << std::setw(20) << std::left << beforeString << std::endl;
    }
    return ostr.str();
}

std::vector<RoleAllocation> RoleAllocationAlgorithm::generateCandidates()
{
    // TODO: caching / profiling?
    std::vector<RoleAllocation> result;
    std::vector<RoleEnum> roles = allAssignableRoles();
    std::vector<PlayerId> players;
    for (auto const& imap: _currentRoleAllocation) players.push_back(imap.first);
    // initialize running RoleAllocation object
    RoleAllocation rc;
    for (auto const &player: players) rc[player] = roles.at(0);
    // construct cartesian product
    int P = players.size();
    int R = roles.size();
    int N = (int)round(pow(R, P));
    int c = 0;
    while (c < N)
    {
        // store
        result.push_back(rc);
        // increment
        int playerIdx = 0;
        int v = (++c) % R;
        int k = c;
        rc[players.at(playerIdx)] = RoleEnum(roles.at(v));
        while (v == 0 && playerIdx+1 < P) // carry
        {
            playerIdx++;
            k = k / R;
            v = k % R;
            rc[players.at(playerIdx)] = RoleEnum(roles.at(v));
        }
    }
    return result;
}

float RoleAllocationAlgorithm::calculatePenalty(RoleAllocation const &candidate)
{
    // what makes a good candidate?
    //   * satisfy rules
    //   * according to own preference
    //   * similar to current
    auto count = roleAllocationToCount(candidate);
    bool validTeam = checkRoleCount(count); // TODO upstream, remove all invalid candidates earlier
    auto myRole = candidate.at(_myId);
    bool validSelf = checkRoleCount(myRole, count.at(myRole));
    bool preferred = (candidate.at(_myId) == _myPreferredRole);
    int difference = 0;
    for (auto const &rp: candidate)
    {
        difference += (_currentRoleAllocation.at(rp.first) != rp.second);
    }
    float penalty = 1000.0 * (!validTeam) + 100.0 * (!validSelf) + 1.0 * difference + 10.0 * !preferred;
    return penalty;
}
