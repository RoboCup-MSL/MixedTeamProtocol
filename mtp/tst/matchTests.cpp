#include "MatchSimulation.hpp"
#include "MatchSimulationChecks.hpp"
#include "TestCase.hpp"
#include <vector>


class MatchTest : public mtp::TestCase { };

TEST_F(MatchTest, TwoMixedTeamsInitialPhase)
{
    // setup
    // 2+3 against 3+2
    MatchSimulation m;
    m.addRobot(mtp::PlayerId(1, 1, 'A'));
    m.addRobot(mtp::PlayerId(1, 2, 'A'));
    m.addRobot(mtp::PlayerId(1, 3, 'B'));
    m.addRobot(mtp::PlayerId(1, 4, 'B'));
    m.addRobot(mtp::PlayerId(1, 5, 'B'));
    m.addRobot(mtp::PlayerId(2, 1, 'B'));
    m.addRobot(mtp::PlayerId(2, 2, 'B'));
    m.addRobot(mtp::PlayerId(2, 3, 'A'));
    m.addRobot(mtp::PlayerId(2, 4, 'A'));
    m.addRobot(mtp::PlayerId(2, 5, 'A'));

    // run
    // 1 simulated second should be enough time for robots to decide on a role allocation
    m.advanceDuration(1.0);

    // assert
    MatchSimulationChecks t(m);
    EXPECT_TRUE(t.checkTeamMemberCount('A', 5));
    EXPECT_TRUE(t.checkTeamMemberCount('B', 5));
    EXPECT_TRUE(t.checkRoleAllocation());
}

TEST_F(MatchTest, WorldModelPosVel)
{
    // setup
    // 2 against 2, no full team needed just to check administration
    MatchSimulation m;
    MatchSimulationChecks t(m);
    for (int id = 1; id <= 4; ++id)
    {
        int shirtId = 1 + (id - 1) % 2; // resp. 1, 2, 1, 2
        char teamId = id < 3 ? 'A' : 'B';
        auto p = mtp::PlayerId(1, shirtId, teamId);
        mtp::Pose pos(id, id);
        mtp::Pose vel(-id, -id);
        m.addRobot(p).setOwnPosVel(pos, vel, 1.0);
        t.setExpectedPosVel(p, pos, vel);
    }

    // run
    m.advanceTicks(2); // first tick SEND and second tick RECV are needed to ensure data arrives

    // assert
    EXPECT_TRUE(t.checkTeamMemberCount('A', 2));
    EXPECT_TRUE(t.checkTeamMemberCount('B', 2));
    EXPECT_TRUE(t.checkWorldModelPosVel()); // check that locations are consistent for the interpretation of each robot
}

TEST_F(MatchTest, RoleAllocationNegotationSingleTeamInvalidCurrentState)
{
    // bug as reported by Jurge from VDL-Robotsports on 2021-05-21

    // setup
    MatchSimulation m;
    m.addRobot(mtp::PlayerId(1, 1, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_GENERIC);
    m.addRobot(mtp::PlayerId(1, 2, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_ASSIST);
    m.addRobot(mtp::PlayerId(1, 3, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_MAIN);
    m.addRobot(mtp::PlayerId(1, 4, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_GENERIC);

    // run
    m.advanceTicks(2); // let the robots learn of each others existenc and possibly already settle on a role allocation

    // again force the given situation
    m.getRobot(mtp::PlayerId(1, 1, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_GENERIC);
    m.getRobot(mtp::PlayerId(1, 2, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_ASSIST);
    m.getRobot(mtp::PlayerId(1, 3, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_MAIN);
    m.getRobot(mtp::PlayerId(1, 4, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_GENERIC);
    m.advanceTicks(2);

    // assert
    MatchSimulationChecks t(m);
    EXPECT_TRUE(t.checkTeamMemberCount('A', 4));
    EXPECT_TRUE(t.checkTeamMemberCount('B', 0));
    EXPECT_TRUE(t.checkRoleAllocation());
}

// TODO: more tests: wm data exchange, refbox signals, preferred roles, jitter/randomness, ...

int main(int argc, char **argv)
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
