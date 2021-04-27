#ifndef _INCLUDED_MIXEDTEAMPROTOCOL_TST_TESTCASE_HPP_
#define _INCLUDED_MIXEDTEAMPROTOCOL_TST_TESTCASE_HPP_

/* Include testframework */
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ::testing;

namespace mtp {

class TestCase : public Test
{
public:
    TestCase()
    {
    };
};

} // end of namespace mtp

#endif
