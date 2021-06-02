// header implemented in this file
#include "int/RoleAllocation.hpp"

// headers from other packages
#include "tprintf.hpp"

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
    // may not call run() from constructor
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
    // input checks
    checkInputs();
    // check if current role allocation is good enough
    if (currentIsOk())
    {
        result = _currentRoleAllocation;
    }
    else
    {
        // run
        error = ERROR_UNINITIALIZED;
        result.clear();
        _run();
    }
    // check result, calculate error code
    checkResult();
}

void RoleAllocationAlgorithm::checkResult()
{
    // check role count against specification
    auto count = roleAllocationToCount(result);
    if (checkRoleCount(count))
    {
        error = 0;
    }
    else
    {
        error = mtp::ERROR_BAD_ROLE;
    }
}

void RoleAllocationAlgorithm::checkInputs() const
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
