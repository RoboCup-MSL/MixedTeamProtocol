cmake_minimum_required(VERSION 3.5.1)

project(MixedTeamProtocol)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake)

enable_testing()

add_subdirectory(rtdb2)
add_subdirectory(mtp)
add_subdirectory(mtrbc)
