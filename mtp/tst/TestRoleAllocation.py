#!/usr/bin/env python3

import os
import traceback
import subprocess
import unittest



# path switching for out-of-source builds, assuming they change into the same directory as the binary
if os.path.isfile('testRoleAllocation'):
    CMD_NAME = './testRoleAllocation'
else:
    # MTP repo situation
    CMD_NAME = os.path.realpath(__file__) + '/../../build/mtp/testRoleAllocation'


def run_cmd(args):
    """Helper function to invoke the command-line interface."""
    cmd = CMD_NAME + " " + " ".join(args)
    output = subprocess.check_output(cmd, shell=True).decode("utf-8")
    return output


class RoleAllocationTestCase(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.maxDiff = None
    def assertMultiLineEqualNoWS(self, got, expected):
        linesGot = [line.strip() for line in got.strip().splitlines()]
        linesExpected = [line.strip() for line in expected.strip().splitlines()]
        self.assertListEqual(linesGot, linesExpected)


# class TestNoArguments(RoleAllocationTestCase):
#     def test(self):
#         # setup
#         args = ["-s MUNKRES"]
#         # run
#         output = run_cmd(args)
#         # check
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#   [self] vendor=1  shirt=1  team=A hash=1     : DEFENDER_GENERIC
#          vendor=1  shirt=2  team=A hash=2     : DEFENDER_MAIN   
#          vendor=1  shirt=3  team=A hash=3     : ATTACKER_ASSIST 
#          vendor=1  shirt=4  team=A hash=4     : ATTACKER_MAIN   
#          vendor=1  shirt=5  team=A hash=5     : GOALKEEPER"""
#         self.assertMultiLineEqualNoWS(output, expected)
#
# class TestNoArguments_MINPREF(RoleAllocationTestCase):
#     def test(self):
#         # setup
#         args = ["-s MINPREF"]
#         # run
#         output = run_cmd(args)
#         # check
#         # minpref assign on order of the roles (goalkeeper is first)        
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#   [self] vendor=1  shirt=1  team=A hash=1     : GOALKEEPER
#          vendor=1  shirt=2  team=A hash=2     : ATTACKER_MAIN   
#          vendor=1  shirt=3  team=A hash=3     : DEFENDER_MAIN 
#          vendor=1  shirt=4  team=A hash=4     : ATTACKER_ASSIST    
#          vendor=1  shirt=5  team=A hash=5     : DEFENDER_GENERIC"""
#         self.assertMultiLineEqualNoWS(output, expected)
#
#
#
# class TestCurrentGoalie(RoleAllocationTestCase):
#     def test(self):
#         self.maxDiff = None
#         # setup
#         args = []
#         # run
#         output = run_cmd(["-c", "GOALKEEPER", "-s MUNKRES"])
#         # check
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#   [self] vendor=1  shirt=1  team=A hash=1     : GOALKEEPER          (current=GOALKEEPER)        
#          vendor=1  shirt=2  team=A hash=2     : DEFENDER_GENERIC
#          vendor=1  shirt=3  team=A hash=3     : DEFENDER_MAIN   
#          vendor=1  shirt=4  team=A hash=4     : ATTACKER_ASSIST 
#          vendor=1  shirt=5  team=A hash=5     : ATTACKER_MAIN"""
#         self.assertMultiLineEqualNoWS(output, expected)
#
# class TestCurrentGoalie_MINREF(RoleAllocationTestCase):
#     def test(self):
#         self.maxDiff = None
#         # setup
#         args = []
#         # run
#         output = run_cmd(["-c", "GOALKEEPER", "-s MINPREF"])
#         # check
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#   [self] vendor=1  shirt=1  team=A hash=1     : GOALKEEPER          (current=GOALKEEPER)        
#          vendor=1  shirt=2  team=A hash=2     : ATTACKER_MAIN
#          vendor=1  shirt=3  team=A hash=3     : DEFENDER_MAIN   
#          vendor=1  shirt=4  team=A hash=4     : ATTACKER_ASSIST 
#          vendor=1  shirt=5  team=A hash=5     : DEFENDER_GENERIC"""
#         self.assertMultiLineEqualNoWS(output, expected)
#
#
#
# class TestCurrentDefender(RoleAllocationTestCase):
#     def test(self):
#         # setup
#         args = []
#         # run
#         output = run_cmd(["-c", "DEFENDER_MAIN", "-s MUNKRES"])
#         # check
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#   [self] vendor=1  shirt=1  team=A hash=1     : DEFENDER_MAIN       (current=DEFENDER_MAIN)     
#          vendor=1  shirt=2  team=A hash=2     : DEFENDER_GENERIC
#          vendor=1  shirt=3  team=A hash=3     : ATTACKER_ASSIST 
#          vendor=1  shirt=4  team=A hash=4     : ATTACKER_MAIN   
#          vendor=1  shirt=5  team=A hash=5     : GOALKEEPER"""
#         self.assertMultiLineEqualNoWS(output, expected)
#
# class TestCurrentDefender_MINPREF(RoleAllocationTestCase):
#     def test(self):
#         # setup
#         args = []
#         # run
#         output = run_cmd(["-c", "DEFENDER_MAIN", "-s MINPREF"])
#         # check
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#   [self] vendor=1  shirt=1  team=A hash=1     : DEFENDER_MAIN       (current=DEFENDER_MAIN)     
#          vendor=1  shirt=2  team=A hash=2     : GOALKEEPER
#          vendor=1  shirt=3  team=A hash=3     : ATTACKER_MAIN 
#          vendor=1  shirt=4  team=A hash=4     : ATTACKER_ASSIST   
#          vendor=1  shirt=5  team=A hash=5     : DEFENDER_GENERIC"""
#         self.assertMultiLineEqualNoWS(output, expected)
#
# class TestNewPreference(RoleAllocationTestCase):
#     """
#     Taking over the goalkeeper role by setting preference.
#     """
#     def test(self):
#         # setup
#         args = []
#         # run
#         output = run_cmd(["-i 4", "-n 4", "-1 GOALKEEPER", "-p GOALKEEPER", "-s MUNKRES"])
#         # check
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#          vendor=1  shirt=1  team=A hash=1     : ATTACKER_MAIN       (current=GOALKEEPER)
#          vendor=1  shirt=2  team=A hash=2     : DEFENDER_MAIN                           
#          vendor=1  shirt=3  team=A hash=3     : ATTACKER_ASSIST                         
#   [self] vendor=1  shirt=4  team=A hash=4     : GOALKEEPER          (preferred=GOALKEEPER)"""
#         self.assertMultiLineEqualNoWS(output, expected)
#
# class TestNewPreference_MINPREF(RoleAllocationTestCase):
#     """
#     Taking over the goalkeeper role by setting preference.
#     """
#     def test(self):
#         # setup
#         args = []
#         # run
#         output = run_cmd(["-i 4", "-n 4", "-1 GOALKEEPER", "-p GOALKEEPER", "-s MINPREF"])
#         # check
# #         expected = """Running algorithm ... done ...
# # Result code: 0
# # Result allocation:
# #          vendor=1  shirt=1  team=A hash=1     : ATTACKER_MAIN       (current=GOALKEEPER)
# #          vendor=1  shirt=2  team=A hash=2     : DEFENDER_MAIN                           
# #          vendor=1  shirt=3  team=A hash=3     : ATTACKER_ASSIST                         
# #   [self] vendor=1  shirt=4  team=A hash=4     : GOALKEEPER          (preferred=GOALKEEPER)"""
#         # DIFFERENT RESULT: player with current role "UNDEFINED" are assigned before players with other current roles
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#          vendor=1  shirt=1  team=A hash=1     : ATTACKER_ASSIST     (current=GOALKEEPER)
#          vendor=1  shirt=2  team=A hash=2     : ATTACKER_MAIN                           
#          vendor=1  shirt=3  team=A hash=3     : DEFENDER_MAIN                         
#   [self] vendor=1  shirt=4  team=A hash=4     : GOALKEEPER          (preferred=GOALKEEPER)"""
#         self.assertMultiLineEqualNoWS(output, expected)
#
#
# class TestThreePlayers(RoleAllocationTestCase):
#     def test(self):
#         # setup
#         args = []
#         # run
#         output = run_cmd(["-n 3", "-s MUNKRES"])
#         # check
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#   [self] vendor=1  shirt=1  team=A hash=1     : DEFENDER_MAIN
#          vendor=1  shirt=2  team=A hash=2     : ATTACKER_MAIN
#          vendor=1  shirt=3  team=A hash=3     : GOALKEEPER"""
#         self.assertMultiLineEqualNoWS(output, expected)
#
# class TestThreePlayers_MINPREF(RoleAllocationTestCase):
#     """
#         3 players test witn minpref.
#         role order is not defined.  
#         minpref assign on order of the roles (goalkeeper is first)
#     """ 
#
#
#     def test(self):
#         # setup
#         args = []
#         # run
#         output = run_cmd(["-n 3", "-s MINPREF"])
#         # check
#         expected = """Running algorithm ... done ...
# Result code: 0
# Result allocation:
#   [self] vendor=1  shirt=1  team=A hash=1     : GOALKEEPER
#          vendor=1  shirt=2  team=A hash=2     : ATTACKER_MAIN
#          vendor=1  shirt=3  team=A hash=3     : DEFENDER_MAIN"""
#         self.assertMultiLineEqualNoWS(output, expected)



class TestTwoPlayers_MUNRES(RoleAllocationTestCase):
    """
        2 players test witn minpref.
        role order is not defined.  
        minpref assign on order of the roles (goalkeeper is first)
    """ 


    def test(self):
        # setup
        args = []
        # run
        output = run_cmd(["-n 2", "-A ATTACKER_MAIN", "-B ATTACKER_ASSIST", "-s MUNKRES"])
        # check
        expected = """Running algorithm ... done ...
Result code: 0
Result allocation:
  [self] vendor=1  shirt=1  team=A hash=1     : ATTACKER_MAIN       (preferred=ATTACKER_MAIN)
         vendor=1  shirt=2  team=A hash=2     : ATTACKER_ASSIST     (preferred=ATTACKER_ASSIST)"""
        print(f">> ====================")
        print(f"{output}")
        print(f"<< ====================")
        self.assertMultiLineEqualNoWS(output, expected)

if __name__ == '__main__':
    # run
    unittest.main()

