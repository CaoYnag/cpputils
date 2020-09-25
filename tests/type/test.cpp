#define BOOST_TEST_MODULE test_types
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <spes/type.h>
using namespace std;
using namespace spes;

BOOST_AUTO_TEST_CASE(EnvTest)
{
	BOOST_REQUIRE_EQUAL(sizeof(s8), 1);
	BOOST_REQUIRE_EQUAL(sizeof(s16), 2);
	BOOST_REQUIRE_EQUAL(sizeof(s32), 4);
	BOOST_REQUIRE_EQUAL(sizeof(s64), 8);

	BOOST_REQUIRE_EQUAL(sizeof(u8), 1);
	BOOST_REQUIRE_EQUAL(sizeof(u16), 2);
	BOOST_REQUIRE_EQUAL(sizeof(u32), 4);
	BOOST_REQUIRE_EQUAL(sizeof(u64), 8);
}