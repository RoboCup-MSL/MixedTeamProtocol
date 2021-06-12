# MixedTeamProtocol
MixedTeamProtocol is an API and library to allow MSL robots to play
together with each other, and in future, even with humans.

# Dependencies
MixedTeamProtocol depends on RTDB. This is handled via `git submodule`.

After cloning, just run the following command once:
`git submodule update --init`

Also required:
* apt packages `libgtest-dev` and `libgmock-dev`
* an installation of the Munkres algorithm
  * manual instructions: see the 3 steps on https://github.com/saebyn/munkres-cpp#usage
  * TODO: automate via (build) script or cmake config?

# Build and test
To build: run `build.sh`.

To test: run `build.sh test`.



