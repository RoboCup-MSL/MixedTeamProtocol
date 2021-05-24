cmake_minimum_required(VERSION 3.5.1)

project(MixedTeamProtocol)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake)

enable_testing()

# enable the following set of compiler flags for uftrace instrumentation and analysis
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

add_subdirectory(rtdb2)
add_subdirectory(mtp)
add_subdirectory(mtrbc)
