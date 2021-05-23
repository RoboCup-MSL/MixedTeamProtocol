#include "MatchSimulation.hpp"
#include "TestCase.hpp"
#include <vector>


class MatchTest : public mtp::TestCase { };

TEST_F(MatchTest, TwoMixedTeamsInitialPhase)
{
    // setup
    // 2+3 against 3+2
    std::vector<mtp::PlayerId> players;
    players.push_back(mtp::PlayerId(1, 1, 'A'));
    players.push_back(mtp::PlayerId(1, 2, 'A'));
    players.push_back(mtp::PlayerId(1, 3, 'B'));
    players.push_back(mtp::PlayerId(1, 4, 'B'));
    players.push_back(mtp::PlayerId(1, 5, 'B'));
    players.push_back(mtp::PlayerId(2, 1, 'B'));
    players.push_back(mtp::PlayerId(2, 2, 'B'));
    players.push_back(mtp::PlayerId(2, 3, 'A'));
    players.push_back(mtp::PlayerId(2, 4, 'A'));
    players.push_back(mtp::PlayerId(2, 5, 'A'));

    MatchSimulation m;
    for (auto &p : players)
    {
        m.addRobot(p);

        mtp::Pose pos;
        pos.x = ((float)rand() / RAND_MAX);
        pos.y = ((float)rand() / RAND_MAX);
        m.setPosVel(p, pos, pos, 0);
    }

    // run
    // 1 simulated second should be enough time for robots to decide on a role allocation
    m.advance(1.0);

    // assert
    EXPECT_TRUE(m.ok());
}

TEST_F(MatchTest, RoleAllocationNegotationSingleTeamInvalidCurrentState)
{
    // bug as reported by Jurge from VDL-Robotsports on 2021-05-21

    // setup
    MatchSimulation m;
    m.addRobot(mtp::PlayerId(1, 1, 'A'));
    m.addRobot(mtp::PlayerId(1, 2, 'A'));
    m.addRobot(mtp::PlayerId(1, 3, 'A'));
    m.addRobot(mtp::PlayerId(1, 4, 'A'));
    // current: 1 and 4 attacker_generic, 2 attacker_assist and 3 attacker_main
    // 

    // run
    // 1 simulated second should be enough time for robots to decide on a role allocation
    m.advance(1.0);

    // assert
    EXPECT_TRUE(m.ok());
}

// TODO: more tests: wm data exchange, refbox signals, preferred roles, jitter/randomness, ...

int main(int argc, char **argv)
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
