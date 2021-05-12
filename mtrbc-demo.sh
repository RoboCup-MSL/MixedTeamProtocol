#!/bin/bash

# RoboCup MSL Refbox 2020 must be started before running this demo

# Start refbox client - this client connects to the refbox and distributes
#                       the commands it receives from the refbox via rtdb
xterm -e ./build/bin/mtrbc --host localhost --config mtrbc/rtdb2_refbox.xml &
sleep 2

# Start example       - this client represents a robot in the team.
#                       the example code may be used as a base to integrate
#                       into a robots code
./build/bin/example-client

