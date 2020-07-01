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


BOOST_AUTO_TEST_CASE(gm_test)
{
	rectf rect(10, 10, 90, 90);
	line2df l1({ 20.f, 20.f }, { 80.f, 80.f });
	line2df l2({ 0.f, 0.f }, { 0.f, 100.f }); // REJECT
	line2df l3({ 1.f, 1.f }, { 100.f, 100.f });
	line2df l4({ 20.f, 1.f }, { 20.f, 100.f });
	line2df l5({ 1.f, 20.f }, { 100.f, 20.f });

	s32 ret = cohen_sutherland(l1, rect);
	BOOST_REQUIRE_EQUAL(ret, CR_ACCEPTED);

	ret = cohen_sutherland(l2, rect);
	BOOST_REQUIRE_EQUAL(ret, CR_REFUSED);

	ret = cohen_sutherland(l3, rect);
	BOOST_REQUIRE_EQUAL(ret, CR_ACCEPTED);

	ret = cohen_sutherland(l4, rect);
	BOOST_REQUIRE_EQUAL(ret, CR_ACCEPTED);

	ret = cohen_sutherland(l5, rect);
	BOOST_REQUIRE_EQUAL(ret, CR_ACCEPTED);
}