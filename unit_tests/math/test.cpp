#define BOOST_TEST_MODULE test_math
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <spes/math.h>
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

BOOST_AUTO_TEST_CASE(rect_intersect_test)
{
	rect ret(0, 0, 0, 0);
	rect base(0, 0, 100, 100);
	rect ni1(-100, 0, -50, 100); // left
	rect ni2(0, -100, 100, -50); // top
	rect ni3(150, 0, 200, 100); // right
	rect ni4(0, 150, 100, 200); // bottom

	bool ni = intersects(base, ni1, ret) || intersects(base, ni2, ret) || intersects(base, ni3, ret) || intersects(base, ni4, ret);
	BOOST_REQUIRE(!ni);

	BOOST_REQUIRE(intersects(base, rect(10, 10, 90, 90), ret)); // inside
	BOOST_REQUIRE(ret == rect(10, 10, 90, 90));

	BOOST_REQUIRE(intersects(base, rect(-10, -10, 110, 110), ret)); // outside
	BOOST_REQUIRE(ret == base);

	BOOST_REQUIRE(intersects(base, rect(50, 50, 110, 110), ret)); // intersects
	BOOST_REQUIRE(ret == rect(50, 50, 100, 100));
}