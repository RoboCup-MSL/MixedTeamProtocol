#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_ROLEALLOCATION_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_ROLEALLOCATION_HPP_

// headers from this package
#include "ext/PlayerId.hpp"
#include "ext/Roles.hpp"
#include "int/Errors.hpp"

namespace mtp
{

RoleCount roleAllocationToCount(RoleAllocation const &roles);

class RoleAllocationAlgorithm
{
public:
    // base class
    // derived implementations should only implement _run()
    
    // behavior:
    // * if algorithm succeeds, then:
    //     a) 'result' allocation is according to the specification (min/max per role)
    //     b) own role preference is satisfied (example: force becoming goalkeeper)
    // * otherwise, a nonzero error is set
    // * if current role allocation is OK, then it will be reused
    RoleAllocationAlgorithm(
        PlayerId const &myId, 
        RoleAllocation const &currentRoleAllocation, 
        RoleEnum const &myPreferredRole = RoleEnum::UNDEFINED,
        float myPreferredRoleFactor = 0.0
        );
    void run(); // cannot be called at construction time, so must be called by client

    // algorithm result
    RoleAllocation result;
    uint8_t error = ERROR_UNINITIALIZED;
    std::string describe() const;

protected:
    PlayerId _myId;
    RoleAllocation _currentRoleAllocation;
    RoleEnum _myPreferredRole = RoleEnum::UNDEFINED;
    float _myPreferredRoleFactor = 0.0;
    bool currentIsOk() const;
    void checkInputs() const;
    void checkResult();
    virtual void _run() = 0; // to be filled in by derived class

}; // end of base class RoleAllocationAlgorithm


class RoleAllocationAlgorithmBruteForce: public RoleAllocationAlgorithm
{
public:
    // brute force algorithm: generate all candidates and select the best one using a penalty calculation
    // such that the algorithm will prefer staying close to current role allocation
    // (minimize role swaps unless absolutely needed)
    using RoleAllocationAlgorithm::RoleAllocationAlgorithm;

    void _run();

private:
    void check() const;
    std::vector<RoleAllocation> generateCandidates();
    float calculatePenalty(RoleAllocation const &candidate);

}; // end of class RoleAllocationAlgorithmBruteForce

class RoleAllocationAlgorithmLinearProgramming: public RoleAllocationAlgorithm
{
public:
    // linear programming: take constraints into account and find the best role
    // this is the most general and flexible algorithm, but perhaps a bit overkill for our use case
    // and the underlying Google OR-tools library is not easy to install
    using RoleAllocationAlgorithm::RoleAllocationAlgorithm;

    void _run();

private:

}; // end of class RoleAllocationAlgorithmLinearProgramming

class RoleAllocationAlgorithmKuhnMunkres: public RoleAllocationAlgorithm
{
public:
    // this variant uses the standard Kuhn-Munkres algorithm (also known as Hungarian algorithm)
    // it is slightly less versatile as LinearProgramming, but much more faster and good enough for now
    // it is also much easier to install than Google OR-tools
    using RoleAllocationAlgorithm::RoleAllocationAlgorithm;

    void _run();

private:

}; // end of class RoleAllocationAlgorithmKuhnMunkres

} // end of namespace mtp

#endif
