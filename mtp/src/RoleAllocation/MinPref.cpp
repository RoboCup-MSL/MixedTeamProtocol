// header partially implemented in this file
#include "int/RoleAllocation.hpp"

// headers from this package
#include "int/Errors.hpp"

// headers from other packages
#include "tprintf.hpp"

// system headers
#include <cmath>
#include <sstream>
#include <iomanip>
#include <set>
#include <iostream>


using namespace mtp;


void RoleAllocationAlgorithmMinPref::_run()
{
    std::cerr << __PRETTY_FUNCTION__ << " - INPUT: " << std::endl;
    std::cerr << "myId =  " << _input.myId.describe() << std::endl;
    std::cerr << "currentRoles = " << std::endl;
    for (auto const& [playerId, currentRole] : _input.currentRoles) {
        std::cerr << "\tplayerId =  " << playerId.describe()  << " - " << roleEnumToString(currentRole) << std::endl;
    }
    std::cerr << "preferredRoles = " << std::endl;
    for (auto const& [playerId, PreferredRole] : _input.preferredRoles) {
        std::cerr << "\tplayerId =  " << playerId.describe()  << " - " << roleEnumToString(PreferredRole.role) << std::endl;
    }

    result.clear();

    std::set<PlayerId> unassigned_players;
    for (auto const &rp: _input.currentRoles)
    {
    	unassigned_players.insert(rp.first);
    }
    auto assignable_roles = allAssignableRoles();
    auto current_roles = _input.currentRoles; // map <key: player id>, value <role>

    //int n_roles = (int)roles.size(); // about 8
    //_input.currentRoles;  : RoleAllocation
    // __input.preferredRoles; : std::map<PlayerId, PreferredRole>

    // result : std::map<PlayerId, RoleEnum> RoleAllocation;

    std::map<RoleEnum, std::vector<PlayerId>>  preferringPlayersPerRole;
    for (auto const& [playerId, preferredRole] : _input.preferredRoles) {
    	if (!preferringPlayersPerRole.count(preferredRole.role)) {
    		// preferred role not in preferringPlayersPerRole yet
    		preferringPlayersPerRole[preferredRole.role] = std::vector<PlayerId>();
    	}
    	preferringPlayersPerRole[preferredRole.role].push_back(playerId);
    }

    // -----------------------------------------------------------------------------
    // Step: assign all minimum roles - till minimum number for the role is reached.
    // -----------------------------------------------------------------------------
    for (RoleEnum role_to_assign : assignable_roles) {
        int minimum_count_for_role = 0, maximum_count_for_role = 0;
        getMinMaxRoleCount(role_to_assign, minimum_count_for_role, maximum_count_for_role);
        if (minimum_count_for_role > 0)
        {
    	// there is a minimum (> 0) for the role defined
        	// assign players to the role till minimum is full-filled
			auto assigned_to_rol_count = 0;

        	// attempt to assign role to player who prefer the role
        	if (preferringPlayersPerRole.count(role_to_assign)) {
				// and preferred by at least 1 player

        		auto players_prefer_role_current_other_role = std::vector<PlayerId>();

				// first assign to players which current role is same as preferred role.
				// for all players which prefer the role
				for (PlayerId playerId : preferringPlayersPerRole[role_to_assign]) {
					if (current_roles.count(playerId)) {
						// player has current role
						if (assigned_to_rol_count < minimum_count_for_role) {
							// minimum role-assignment is not full-filled
							assigned_to_rol_count++;
							result[playerId] = role_to_assign;	// assign role to robot
							unassigned_players.erase(playerId);	// remove player from available player list
						}
					} else {
						players_prefer_role_current_other_role.push_back(
								playerId); // player prefer role but has current different role
					}
				}

			    // for all players which prefer the role but have different current role
				for (PlayerId playerId : players_prefer_role_current_other_role) {
					if (assigned_to_rol_count < minimum_count_for_role) {
						// minimum role-assignment is not full-filled
						assigned_to_rol_count++;
						result[playerId] = role_to_assign; // assign role to robot
						unassigned_players.erase(playerId); // remove player from available player list
					}
				}
			}

        	// assign role to players which currently have the same role (but not prefer it) till minimum role-assignment is full-filled
        	if (assigned_to_rol_count < minimum_count_for_role)
        	{
        		// minimum for role not full filled yet.
        		for (PlayerId playerId : unassigned_players)
        		{
        			if (current_roles[playerId] == role_to_assign) {
    					if (assigned_to_rol_count < minimum_count_for_role) {
    						// role minimum for not reached
    						assigned_to_rol_count++;
    						result[playerId] = role_to_assign; // assign role to robot
    					}
        			}
        		}
                for (auto const& [playerId, assigned_role] : result) {
                	if (unassigned_players.count(playerId)) {
                		unassigned_players.erase(playerId); // remove player from unassigned player list
                	}
                }
        	}

        	// assign role to unassigned players till minimum role-assignment is full-filled
        	if (assigned_to_rol_count < minimum_count_for_role)
        	{
        		// minimum for role not full filled yet.
        		// assign first role to players with current role UNDEFINED
        		for (PlayerId playerId : unassigned_players)
        		{
        			if (current_roles[playerId] == RoleEnum::UNDEFINED) {
    					if (assigned_to_rol_count < minimum_count_for_role) {
    						// role minimum for not reached
    						assigned_to_rol_count++;
    						result[playerId] = role_to_assign; // assign role to robot
    					}
        			}
        		}
                for (auto const& [playerId, assigned_role] : result) {
                	if (unassigned_players.count(playerId)) {
                		unassigned_players.erase(playerId); // remove player from unassigned player list
                	}
                }
        	}


			if (assigned_to_rol_count < minimum_count_for_role)
			{
        		for (PlayerId playerId : unassigned_players)
        		{
					if (assigned_to_rol_count < minimum_count_for_role) {
						// role minimum for not reached
						assigned_to_rol_count++;
						result[playerId] = role_to_assign; // assign role to robot
					}
        		}
                for (auto const& [playerId, assigned_role] : result) {
                	if (unassigned_players.count(playerId)) {
                		unassigned_players.erase(playerId); // remove player from unassigned player list
                	}
                }
        	}
        }
    }
    // -----------------------------------------------------------------------------
    // all minimum role assignments filled in with the set of available player


    // -----------------------------------------------------------------------------
    // Step: assign preferred roles - till maximum number for the role is reached.
    // -----------------------------------------------------------------------------
    for (RoleEnum role_to_assign : assignable_roles)
    {
        int minimum_count_for_role = 0, maximum_count_for_role = 0;
        getMinMaxRoleCount(role_to_assign, minimum_count_for_role, maximum_count_for_role);
        int players_for_role = 0;

        // get number of players already assigned to role in previous steps
        for (auto const& [playerId, assigned_role] : result) {
        	if (assigned_role == role_to_assign) {
        		players_for_role++;
        	}
        }

        // assign players to role maximum which prefer the role and currently have the role
        for (PlayerId playerId : preferringPlayersPerRole[role_to_assign]) {
        	if (players_for_role < maximum_count_for_role) {
        		if (unassigned_players.count(playerId)) {
					if (current_roles.count(playerId)) {
						// player has current role, not assigned and prefer the role.
						players_for_role++;
						result[playerId] = role_to_assign; // assign role to robot
						unassigned_players.erase(playerId); // remove player from unassigned player list
					}
        		}
        	}
        }

        // assign players to role maximum which prefer the role (no check if current role is same)
        for (PlayerId playerId : preferringPlayersPerRole[role_to_assign]) {
        	if (players_for_role < maximum_count_for_role) {
        		if (unassigned_players.count(playerId)) {
					// player has current role, not assigned and prefer the role.
					players_for_role++;
					result[playerId] = role_to_assign; // assign role to robot
					unassigned_players.erase(playerId); // remove player from unassigned player list
        		}
        	}
        }
    }

    // -----------------------------------------------------------------------------
    // Step: assign roles - till maximum number for the role is reached.
    // -----------------------------------------------------------------------------
    //    Per robot Loop over de roles, indien maximum voor rol niet bereikt is, ken de role toe
    //    Step 4: indien robot nog geen rol heeft, dan zijn er meer robots dan het totaal van de max roles  ==> config fout


    for (RoleEnum role_to_assign : assignable_roles)
    {
        int minimum_count_for_role = 0, maximum_count_for_role = 0;
        getMinMaxRoleCount(role_to_assign, minimum_count_for_role, maximum_count_for_role);
        int players_for_role = 0;

        // get number of players already assigned to role in previous steps
        for (auto const& [playerId, assigned_role] : result) {
        	if (assigned_role == role_to_assign) {
        		players_for_role++;
        	}
        }

        if (players_for_role < maximum_count_for_role) {
        	for (PlayerId playerId : unassigned_players) {
                if (players_for_role < maximum_count_for_role) {
                	players_for_role++;
                	result[playerId] = role_to_assign; // assign role to robot
                }
        	}
        }

        for (auto const& [playerId, assigned_role] : result) {
        	if (unassigned_players.count(playerId)) {
        		unassigned_players.erase(playerId); // remove player from unassigned player list
        	}
        }
    }

    std::cerr << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cerr << __PRETTY_FUNCTION__ << " - OUTPUT at end -> line:" << __LINE__ << std::endl;
    for (auto const& [playerId, assigned_role] : result) {
        std::cerr << "\tplayerId =  " << playerId.describe()  << " - " << roleEnumToString(assigned_role) << std::endl;
    }
    std::cerr << std::flush;
}


