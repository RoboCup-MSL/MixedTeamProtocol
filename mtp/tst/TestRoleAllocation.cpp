// headers from this package
#include "int/RoleAllocation.hpp"

// standard/system headers
#include <iostream>
#include <boost/program_options.hpp>



int main(int argc, char **argv)
{
    // parse options
    namespace po = boost::program_options;
    po::options_description desc("Test the role allocation algorithm (single tick). Can customize up to 4 teammembers. Default: a full 5-robot team.\nOptions");
    desc.add_options()
        ("help,h", "produce help message")
        ("my-id,i", po::value<int>()->default_value(1), "my id")
        ("my-role,c", po::value<std::string>()->default_value("UNDEFINED"), "my current role")
        ("my-preference,p", po::value<std::string>()->default_value("UNDEFINED"), "my preferred role")
        ("num-players,n", po::value<int>()->default_value(5), "number of players in team")
        ("teammember-1,1", po::value<std::string>(), "role of first team member")
        ("teammember-2,2", po::value<std::string>(), "role of second team member")
        ("teammember-3,3", po::value<std::string>(), "role of third team member")
        ("teammember-4,4", po::value<std::string>(), "role of fourth team member")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    // checks: num_players cannot be smaller than specified role
    int num_players = vm.at("num-players").as<int>();
    for (int num_other = 4; num_other >= 1; --num_other)
    {
        std::string option_check = "teammember-" + std::to_string(num_other);
        if ((num_players <= num_other) && vm.count(option_check))
        {
            std::cerr << "invalid combination of options: num_players=" << num_players << " and " << option_check << std::endl;
            return 1;
        }
    }

    // setup the input players and their preset roles
    mtp::PlayerId myId(1, vm.at("my-id").as<int>());
    mtp::RoleAllocation currentRoles;
    currentRoles[myId] = mtp::roleStringToEnum(vm.at("my-role").as<std::string>());
    std::set<int> assignedIds = {myId.shirtId};
    for (int teammember = 1; teammember < num_players; ++teammember)
    {
        // determine new id
        int teammemberShirtId = teammember;
        while (assignedIds.count(teammemberShirtId)) teammemberShirtId++;
        assignedIds.insert(teammemberShirtId);
        // assign role
        auto role = mtp::RoleEnum::UNDEFINED;
        std::string k = "teammember-" + std::to_string(teammember);
        if (vm.count(k)) role = mtp::roleStringToEnum(vm.at(k).as<std::string>());
        currentRoles[mtp::PlayerId(1, teammemberShirtId)] = role;
    }

    // run the algorithm
    std::cout << "Running algorithm ..." << std::flush;
    mtp::RoleEnum myPreferredRole = mtp::roleStringToEnum(vm.at("my-preference").as<std::string>());
    float myPreferredRoleFactor = 0.0;
    if (myPreferredRole != mtp::RoleEnum::UNDEFINED)
    {
        myPreferredRoleFactor = 1.0; // TODO: allow more options?
    }
    auto r = mtp::RoleAllocationAlgorithmBruteForce(myId, currentRoles, myPreferredRole, myPreferredRoleFactor);
    r.run();
    std::cout << " done ..." << std::endl;

    // print algorithm result versus input
    std::cout << r.describe() << std::endl;

    return 0;
}
