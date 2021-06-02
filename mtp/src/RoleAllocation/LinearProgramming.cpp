// header implemented in this file
#include "int/RoleAllocation.hpp"

// headers from this package
#include "int/Errors.hpp"

// headers from other packages
#include "tprintf.hpp"

// system headers
#include <cmath>
#include <sstream>
#include <iomanip>

// ORtools optimization toolbox
#include "ortools/linear_solver/linear_solver.h"


using namespace mtp;
using namespace operations_research;


void RoleAllocationAlgorithmLinearProgramming::_run()
{
    // setup solver
    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
    if (!solver)
    {
        throw std::runtime_error("SCIP solver unavailable");
    }

    // we have P players and R roles
    std::vector<PlayerId> players;
    for (auto const &rp: _currentRoleAllocation)
    {
        players.push_back(rp.first);
    }
    int P = (int)players.size(); // typically 5, may grow towards 11 in future
    auto roles = allAssignableRoles();
    int R = (int)roles.size(); // about 8
    //tprintf("P=%d R=%d", P, R);

    // a role assignment is a pair (player, role) with a boolean value
    // so all possible role assignments are in the cartesian product, size P*R
    // these are the variables to be solved for
    std::vector<std::vector<const MPVariable*>> variables(P, std::vector<const MPVariable*>(R));
    for (int p = 0; p < P; ++p)
    {
        for (int r = 0; r < R; ++r)
        {
            variables.at(p).at(r) = solver->MakeIntVar(0, 1, "");
        }
    }

    // set the constraints
    // each player is assigned to at exactly one role
    for (int p = 0; p < P; ++p)
    {
        LinearExpr sum;
        for (int r = 0; r < R; ++r)
        {
            sum += variables.at(p).at(r);
        }
        solver->MakeRowConstraint(sum == 1);
    }
    // role minimum- and maximum constraints
    for (int r = 0; r < R; ++r)
    {
        LinearExpr sum;
        int minCount = 0, maxCount = 0;
        getMinMaxRoleCount(roles.at(r), minCount, maxCount);
        for (int p = 0; p < P; ++p)
        {
            sum += variables.at(p).at(r);
        }
        solver->MakeRowConstraint(sum >= minCount);
        solver->MakeRowConstraint(sum <= maxCount);
    }

    // objective function - TODO, for now 1
    MPObjective* const objective = solver->MutableObjective();
    //std::vector<std::vector<const MPVariable*>> variables(P, std::vector<const MPVariable*>(R));
    for (int p = 0; p < P; ++p)
    {
        for (int r = 0; r < R; ++r)
        {
            objective->SetCoefficient(variables.at(p).at(r), 1); // TODO
        }
    }
    objective->SetMinimization();

    // solve
    const MPSolver::ResultStatus result_status = solver->Solve();

    // check and convert solution
    result.clear();
    if (result_status != MPSolver::OPTIMAL && result_status != MPSolver::FEASIBLE)
    {
        return; // no solution found
    }
    for (int p = 0; p < P; ++p)
    {
        for (int r = 0; r < R; ++r)
        {
            if (variables.at(p).at(r)->solution_value() > 0.5)
            {
                result[players.at(p)] = roles.at(r);
            }
        }
    }
}


