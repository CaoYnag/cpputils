#define BOOST_TEST_MODULE test_types
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "../../cpputils/color.h"
using namespace std;
using namespace spes;
using namespace spes::color;

BOOST_AUTO_TEST_CASE(EnvTest)
{
	BOOST_REQUIRE_EQUAL(Colors::RED.r, 255);
	BOOST_REQUIRE_EQUAL(Colors::RED.g, 0);
	BOOST_REQUIRE_EQUAL(Colors::RED.b, 0);

	BOOST_REQUIRE_EQUAL(Colors::GREEN.r, 0);
	BOOST_REQUIRE_EQUAL(Colors::GREEN.g, 255);
	BOOST_REQUIRE_EQUAL(Colors::GREEN.b, 0);

	BOOST_REQUIRE_EQUAL(Colors::BLUE.r, 0);
	BOOST_REQUIRE_EQUAL(Colors::BLUE.g, 0);
	BOOST_REQUIRE_EQUAL(Colors::BLUE.b, 255);
}