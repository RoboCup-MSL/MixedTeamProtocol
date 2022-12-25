# MixedTeamProtocol
MixedTeamProtocol is an API and library to allow MSL robots to play
together with each other, and in future, even with humans.

# Dependencies
MixedTeamProtocol depends on RTDB and on munkres-cpp. This is handled via `git submodule`.

After cloning, just run the following command once:
`git submodule update --init`

Also required:
* apt packages `libgtest-dev` and `libgmock-dev`
* pip package `lmdb` and `pause` and `hexdump`

# Build and test
To build: run `build.py`.

To test: run `build.py --test`.



