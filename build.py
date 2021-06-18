#!/usr/bin/env python3
# make wrapper


import argparse
import os, shutil
import time


def parse_args():
    # Argument parsing.
    descriptionTxt = 'MTP Build wrapper script.\n'
    exampleTxt = 'Examples:\n   build.py # just build\n   build.py -c --test # clean build and test\n   build.py -c --trace MatchTest.TwoMixedTeamsInitialPhase\n\n'
    parser     = argparse.ArgumentParser(description=descriptionTxt, epilog=exampleTxt, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-c', '--pre-clean', help='cleanup before build', action='store_true')
    parser.add_argument('-n', '--dry-run', help='print commands instead of executing them', action='store_true')
    parser.add_argument('-j', '--jobs', help='number of parallel build jobs (threads) to run', type=int, default=4)
    parser.add_argument('-v', '--verbose', help='call make with its verbose option, which is useful to debug include directories and such', action='store_true')
    parser.add_argument('-d', '--debug', help='enable debugging symbols in build', action='store_true')
    parser.add_argument('-t', '--test', help='also perform tests', action='store_true')
    parser.add_argument('--trace', help='build with tracing enabled and test a specific test case', type=str)
    parser.add_argument('--uftrace-opts', help='only if --trace is used: pass these options to uftrace', type=str, default='-a -t 1us -A printf@arg1/s -A _tprintf_wrap@arg1/s')
    return parser.parse_args()


class Commands:
    """
    Helper class to define a sequence of commands to run.
    Useful for the --dry-run option: just print instead of execute.
    
    Underwater uses some auxiliary functions, for instance to change / clear directories.
    """
    def __init__(self):
        self._commands = []
    def show(self):
        for (cmd, args) in self._commands:
            print(cmd, ' '.join(args))
    def run(self):
        cmd_map = {}
        cmd_map['cd'] = os.chdir
        def rmtree(d):
            if os.path.isdir(d):
                shutil.rmtree(d)
        cmd_map['rmtree'] = rmtree
        def mkdir(d):
            if not os.path.isdir(d):
                os.mkdir(d)
        cmd_map['mkdir'] = mkdir
        cmd_map['rm'] = os.remove
        ok = True
        for (cmd, args) in self._commands:
            if cmd in cmd_map:
                cmd_map[cmd](*args)
            else:
                s = ' '.join([cmd] + list(args))
                r = os.system(s)
                if r != 0:
                    ok = False
                    break
        return ok
    def add(self, command, *args):
        self._commands.append((command, args))


def run(args):
    # check options
    do_clean = args.pre_clean
    do_build = True

    # construct sequence of commands
    # (useful for option --dry-run, in case people are interested in the underwater commands)
    commands = Commands()

    # go to repo root folder
    # this needs to be done always, even in dry-run mode, to properly detect folders for cleanup
    repo_root_folder = os.path.dirname(os.path.realpath(__file__))
    commands.add('cd', repo_root_folder)
    os.chdir(repo_root_folder)

    # pre-clean phase: remove build folders
    if do_clean:
        commands.add('rmtree', 'build')

    # go into build folder, create if needed
    commands.add('mkdir', 'build')
    commands.add('cd', 'build')

    # build phase
    if do_build:
        # determine options for cmake and make
        cmake_opts = ''
        if args.trace:
            cmake_opts += '-E env CXXFLAGS="-pg" cmake'
        if args.debug:
            cmake_opts += '-DCMAKE_BUILD_TYPE=Debug'
            # TODO: how to set compiler #define DEBUG? see MixedTeamProtocolImpl.cpp
        make_opts = '-j' + str(args.jobs)
        if args.verbose:
            make_opts += ' VERBOSE=1'
        # run cmake to (re-)generate makefiles
        commands.add('cmake', cmake_opts, '..')
        # run make
        commands.add('make', make_opts)

    # test phase
    if args.test:
        commands.add('export GTEST_COLOR=1 ; make ARGS="-VV --timeout 120" test')
    if args.trace:
        commands.add('uftrace', args.uftrace_opts, 'record', 'mtp/matchTests', '--gtest_filter=' + args.trace, ' || true') # continue even if failing
        jsonfile = '/tmp/uftrace.json'
        htmlfile = '/tmp/uftrace.html'
        commands.add('uftrace dump', '--chrome', '>', jsonfile)
        commands.add(repo_root_folder + '/mtp/tst/trace2html.py', '--view', jsonfile, htmlfile)

    # now run the sequence of commands, unless dry-run
    if args.dry_run:
        commands.show()
    else:
        ok = commands.run()


if __name__ == '__main__':
    # run
    run(parse_args())

