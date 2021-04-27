#include "MatchSimulation.hpp"
#include "TestCase.hpp"


class MatchTest : public mtp::TestCase { };

TEST_F(MatchTest, TwoMixedTeamsInitialPhase)
{
    // setup
    // 2+3 against 3+2
    MatchSimulation m;
    m.addRobot(1, 1, 'A');
    m.addRobot(1, 2, 'A');
    m.addRobot(1, 3, 'B');
    m.addRobot(1, 4, 'B');
    m.addRobot(1, 5, 'B');
    m.addRobot(2, 1, 'B');
    m.addRobot(2, 2, 'B');
    m.addRobot(2, 3, 'A');
    m.addRobot(2, 4, 'A');
    m.addRobot(2, 5, 'A');

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
