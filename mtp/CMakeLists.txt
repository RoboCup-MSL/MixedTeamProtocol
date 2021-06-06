cmake_minimum_required(VERSION 3.5.1)

project(mixedTeamProtocol)

find_package(RtDB)
find_package(Boost COMPONENTS thread program_options REQUIRED QUIET)

# depend on Google OR-tools
set(ORTOOLS_ROOT "~/hobby/or-tools_Ubuntu-20.04-64bit_v9.0.9048") # TODO this won't work for Rob... need submodule, or..? 
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${ORTOOLS_ROOT})
find_package(ortools REQUIRED)
include_directories( # TODO: why isn't this exported?
    ${ORTOOLS_INCLUDE_DIRS}
)

set(CMAKE_CXX_STANDARD 17) # for std::filesystem
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Werror")

include_directories("include")

add_library(mixedTeamProtocol
    src/MixedTeamProtocol.cpp
    src/MixedTeamProtocolImpl.cpp
    src/Communication.cpp
    src/AdapterRTDB.cpp
    src/Referee.cpp
    src/Roles.cpp
    src/RoleAllocation/RoleAllocation.cpp
    src/RoleAllocation/BruteForce.cpp
    src/RoleAllocation/LinearProgramming.cpp
    src/PlayerId.cpp
    src/Player.cpp)
add_library(MTP::mixedTeamProtocol ALIAS mixedTeamProtocol)
target_link_libraries(mixedTeamProtocol RtDBrtdb ortools)
target_include_directories(mixedTeamProtocol
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ext>
)

# test build targets

add_library(testLib tst/MatchSimulation.cpp tst/MatchSimulationChecks.cpp tst/RobotClient.cpp)
target_link_libraries(testLib mixedTeamProtocol)

add_executable(matchTests tst/matchTests.cpp)
target_link_libraries(matchTests testLib gtest ${Boost_LIBRARIES} RtDButils)

add_executable(testRoleAllocation tst/TestRoleAllocation.cpp)
target_link_libraries(testRoleAllocation mixedTeamProtocol ${Boost_LIBRARIES})



# test suite

add_test(matchTests matchTests)
set_tests_properties(matchTests PROPERTIES TIMEOUT 10)

add_test(TestRoleAllocation python3 ${CMAKE_CURRENT_LIST_DIR}/tst/TestRoleAllocation.py)
set_tests_properties(TestRoleAllocation PROPERTIES TIMEOUT 10)

