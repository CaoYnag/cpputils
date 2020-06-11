#define BOOST_TEST_MODULE test_types
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "../../cpputils/math.h"
#include <vector>
using namespace std;
using namespace spes::math;

BOOST_AUTO_TEST_CASE(statistics_test)
{
	vector<int> is = {0, 1, 2, 3, 4};//4 1 0 1 4
	double a = avg(is);
	double v = variance(is, a);

	BOOST_REQUIRE_EQUAL(a, 2);
	BOOST_REQUIRE_EQUAL(v, 2);
}