cmake_minimum_required(VERSION 3.5.1)

project(mtrbc)

# system dependencies
find_package(Boost REQUIRED COMPONENTS thread QUIET)
find_package(Qt5 REQUIRED COMPONENTS Core)

# library
add_library(mtrbclient STATIC
    RefboxCommand.cpp
    RefboxProtocol2020Client.cpp
)

target_link_libraries(mtrbclient
    ${Boost_THREAD_LIBRARY}
    Qt5::Core
)

# binary
add_executable(mtrbc main.cpp)
target_link_libraries(mtrbc
    mtrbclient
)

