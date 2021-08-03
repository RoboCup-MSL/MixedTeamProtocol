#include "MatchSimulation.hpp"
#include "MatchSimulationChecks.hpp"
#include "TestCase.hpp"

class ClearHistoryTest : public mtp::TestCase { };

TEST_F(ClearHistoryTest, Test1)
{
    // setup
    MatchSimulation m;
    MatchSimulationChecks t(m);
    EXPECT_TRUE(t.checkTeamMemberCount('A', 0));

    m.addRobot(mtp::PlayerId(1, 1, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_GENERIC);
    m.addRobot(mtp::PlayerId(1, 2, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_ASSIST);
    m.addRobot(mtp::PlayerId(1, 3, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_MAIN);
    m.addRobot(mtp::PlayerId(1, 4, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_GENERIC);

    // run
    m.advanceTicks(2); // let the robots learn of each others existence and possibly already settle on a role allocation

    // assert
    EXPECT_TRUE(t.checkTeamMemberCount('A', 4));
}

TEST_F(ClearHistoryTest, Test2)
{
    // setup
    MatchSimulation m;
    MatchSimulationChecks t(m);
    EXPECT_TRUE(t.checkTeamMemberCount('A', 0));

    m.addRobot(mtp::PlayerId(1, 1, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_GENERIC);
    m.addRobot(mtp::PlayerId(1, 2, 'A')).setCurrentRole(mtp::RoleEnum::ATTACKER_ASSIST);

    // run
    m.advanceTicks(2);

    // assert
    EXPECT_TRUE(t.checkTeamMemberCount('A', 2));
}

int main(int argc, char **argv)
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
