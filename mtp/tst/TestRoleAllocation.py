#!/usr/bin/env python3

import traceback
import subprocess
import unittest


CMD_NAME = "../../build/mtp/testRoleAllocation"


def run_cmd(args):
    """Helper function to invoke the command-line interface."""
    cmd = CMD_NAME + " " + " ".join(args)
    output = subprocess.check_output(cmd, shell=True).decode("utf-8")
    return output


class TestNoArguments(unittest.TestCase):
    def test(self):
        # setup
        args = []
        # run
        output = run_cmd(args)
        # check
        expected = """Running algorithm ... done ...
Result code: 0
Result allocation:
  [self] vendor=1  shirt=1  team=A hash=1     : DEFENDER_MAIN       (UNDEFINED)         
         vendor=1  shirt=2  team=A hash=2     : ATTACKER_GENERIC    (UNDEFINED)         
         vendor=1  shirt=3  team=A hash=3     : ATTACKER_ASSIST     (UNDEFINED)         
         vendor=1  shirt=4  team=A hash=4     : ATTACKER_MAIN       (UNDEFINED)         
         vendor=1  shirt=5  team=A hash=5     : GOALKEEPER          (UNDEFINED)"""
        self.assertEqual(output.strip(), expected.strip())


class TestCurrentGoalie(unittest.TestCase):
    def test(self):
        self.maxDiff = None
        # setup
        args = []
        # run
        output = run_cmd(["-c", "GOALKEEPER"])
        # check
        expected = """Running algorithm ... done ...
Result code: 0
Result allocation:
  [self] vendor=1  shirt=1  team=A hash=1     : GOALKEEPER          (GOALKEEPER)        
         vendor=1  shirt=2  team=A hash=2     : DEFENDER_MAIN       (UNDEFINED)         
         vendor=1  shirt=3  team=A hash=3     : ATTACKER_GENERIC    (UNDEFINED)         
         vendor=1  shirt=4  team=A hash=4     : ATTACKER_ASSIST     (UNDEFINED)         
         vendor=1  shirt=5  team=A hash=5     : ATTACKER_MAIN       (UNDEFINED)"""
        self.assertEqual(output.strip(), expected.strip())


class TestCurrentDefender(unittest.TestCase):
    def test(self):
        # setup
        args = []
        # run
        output = run_cmd(["-c", "DEFENDER_MAIN"])
        # check
        expected = """Running algorithm ... done ...
Result code: 0
Result allocation:
  [self] vendor=1  shirt=1  team=A hash=1     : DEFENDER_MAIN       (DEFENDER_MAIN)     
         vendor=1  shirt=2  team=A hash=2     : ATTACKER_GENERIC    (UNDEFINED)         
         vendor=1  shirt=3  team=A hash=3     : ATTACKER_ASSIST     (UNDEFINED)         
         vendor=1  shirt=4  team=A hash=4     : ATTACKER_MAIN       (UNDEFINED)         
         vendor=1  shirt=5  team=A hash=5     : GOALKEEPER          (UNDEFINED)"""
        self.assertEqual(output.strip(), expected.strip())


if __name__ == '__main__':
    # run
    unittest.main()
