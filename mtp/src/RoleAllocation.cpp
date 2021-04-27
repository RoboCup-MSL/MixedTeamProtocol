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
    RoleAllocation const &currentRoles,
    std::string const &myPreferredRoleString,
    float myPreferredRoleFactor
    )
:
    _myId(myId),
    _currentRoles(currentRoles),
    _myPreferredRoleString(myPreferredRoleString),
    _myPreferredRoleFactor(myPreferredRoleFactor)
{
    // initialize output
    result.clear();
    error = 0;
    // run
    run();
}

bool RoleAllocationAlgorithm::currentIsOk() const
{
    // check own role preference
    if (_myPreferredRoleFactor > 0.0)
    {
        RoleEnum myPreferredRole = roleStringToEnum(_myPreferredRoleString);
        if (_currentRoles.at(_myId) != myPreferredRole) return false;
    }
    // check role count against specification
    auto count = roleAllocationToCount(_currentRoles);
    return checkRoleCount(count);
}

void RoleAllocationAlgorithm::run()
{
    // check if current situation is OK
    if (currentIsOk())
    {
        result = _currentRoles;
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
    //printf("bestPenalty=%e\n", bestPenalty);
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
        ostr << "  " << selfString << " " << rolePair.first.describe() << ": " << std::setw(20) << std::left << mtp::roleEnumToString(rolePair.second);
        std::string beforeString = "(" + mtp::roleEnumToString(_currentRoles.at(rolePair.first)) + ")";
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
    for (auto const& imap: _currentRoles) players.push_back(imap.first);
    // initialize running RoleAllocation object
    RoleAllocation rc;
    for (auto const &player: players) rc[player] = roles.at(0);
    // construct cartesian product
    int P = players.size();
    int R = roles.size();
    int N = (int)round(pow(R, P));
    int c = 0;
    //printf("prepare dimensions P=%d R=%d N=%d\n", P, R, N);
    while (c < N)
    {
        // store
        //printf("storing Result:");
        //for (int j = 0; j < P; ++j) printf(" %s", roleEnumToString(rc[players[j]]).c_str());
        //printf("\n");
        result.push_back(rc);
        // increment
        int playerIdx = 0;
        int v = (++c) % R;
        int k = c;
        rc[players[playerIdx]] = RoleEnum(roles[v]);
        //printf("increment1 c=%-6d p=%-6d v=%-6d k=%-6d\n", c, playerIdx, v, k);
        while (v == 0 && playerIdx < P) // carry
        {
            playerIdx++;
            k = k / R;
            v = k % R;
            //printf("increment2 c=%-6d p=%-6d v=%-6d k=%-6d\n", c, playerIdx, v, k);
            rc[players[playerIdx]] = RoleEnum(roles[v]);
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
    bool valid = checkRoleCount(roleAllocationToCount(candidate)); // TODO upstream, remove all invalid candidates earlier
    bool preferred = (candidate.at(_myId) == roleStringToEnum(_myPreferredRoleString));
    int difference = 0;
    for (auto const &rp: candidate)
    {
        difference += (_currentRoles.at(rp.first) != rp.second);
    }
    float penalty = 1000.0 * (!valid) + 10.0 * difference + 1.0 * !preferred;
    return penalty;
}

/*
    // algorithm result
class 
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

    void currentIsOk();
    void run();

}; // end of class RoleAllocationAlgorithm


bool MixedTeamProtocolImpl::analyzePlayerRoles(std::vector<RoleEnum> &candidates)
{
    // returns true if recalculation is needed
    candidates.clear();
    // count current roles
    std::map<RoleEnum, int> roleCount = countCurrentRoles();
    // check if all OK
    if (checkRoleCount(roleCount)) // TODO roles:: namespace?
    {
        return false;
    }
    // determine of our role is part of the problem
    if (checkRoleCount(_role, roleCount.at(_role)))
    {
        // one might think that since this robot is not contributing to the conflict, it is not needed to reassign
        // but it can happen that all robots are ending up in 'generic support roles ...
        _error |= ERROR_BAD_ROLE_ELSEWHERE;
        return true;
    }
    // this robot is part of the conflict
    _error |= ERROR_BAD_ROLE_SELF;
    // calculate available roles
    candidates = calculateAvailableRoles(roleCount);
    return true;
}

void MixedTeamProtocolImpl::calculateOwnRole()
{
    // check if roles are already OK
    // if so, return as a side product the candidate roles to choose from
    std::vector<RoleEnum> candidates;
    bool needRecalculate = analyzePlayerRoles(candidates);
    if (!needRecalculate) return;
    // determine new role
    // first, take own preference into account (random)
    std::uniform_real_distribution<float> distribution(0.0, 1.0);
    float random_value = distribution(_rng);
    if (random_value < _preferredRoleFactor)
    {
        if (_role != roleStringToEnum(_preferredRoleString))
        {
            tprintf("[%3d] assigned new role %20s to %5d according to preference", _error, _preferredRoleString.c_str(), _id.hash());
        }
        _role = roleStringToEnum(_preferredRoleString);
        return;
    }
    // select one of the candidate roles at random
    // TODO: consider at setpiece-start that the randomized attacker-main might be far away from the ball,
    //       should this algorithm take such world state logic into account? 
    //       Or do we leave it up to clients to use the 'preferredRole' interface
    //       @Erik, what do you think?
    if (candidates.size())
    {
        std::shuffle(std::begin(candidates), std::end(candidates), _rng);
        _role = candidates.at(0);
        std::string alternatives_str;
        for (auto i = candidates.begin() + 1; i != candidates.end(); ++i) { alternatives_str += roleEnumToString(*i) + " "; }
        tprintf("[%3d] assigned new role %20s to %5d by random selection, alternatives were %s", _error, roleEnumToString(_role).c_str(), _id.hash(), alternatives_str.c_str());
    }
    else
    {
        // fallback, remain in error state
        _role = RoleEnum::UNDEFINED;
    }
}
*/   
