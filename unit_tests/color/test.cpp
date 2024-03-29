﻿#define BOOST_TEST_MODULE test_types
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <spes/color.h>
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

BOOST_AUTO_TEST_CASE(rgb_hsv)
{
    constexpr const f32 PI = 3.141592654589793;
    constexpr const f32 deg60 = PI / 3;
    auto black_hsv = to_hsv(Colors::BLACK);
    auto black_rev = from_hsv(black_hsv);
    BOOST_REQUIRE_CLOSE_FRACTION(black_hsv.h, .0f, .0000001f);
    BOOST_REQUIRE_CLOSE_FRACTION(black_hsv.s, .0f, .0000001f);
    BOOST_REQUIRE_CLOSE_FRACTION(black_hsv.v, .0f, .0000001f);
    BOOST_REQUIRE_EQUAL(black_rev, Colors::BLACK);
    BOOST_REQUIRE(black_rev == Colors::BLACK);
    cout << "BLACK: " << Colors::BLACK << " reved: " << black_rev << endl;

    auto red_hsv = to_hsv(Colors::RED);
    auto red_rev = from_hsv(red_hsv);
    BOOST_REQUIRE_CLOSE_FRACTION(red_hsv.h, .0f, .0000001f);
    BOOST_REQUIRE_CLOSE_FRACTION(red_hsv.s, 1.0f, .0000001f);
    BOOST_REQUIRE_CLOSE_FRACTION(red_hsv.v, 1.0f, .0000001f);
    BOOST_REQUIRE_EQUAL(red_rev, Colors::RED);
    cout << "RED  : " << Colors::RED << " reved: " << red_rev << endl;


    auto green_hsv = to_hsv(Colors::GREEN);
    auto green_rev = from_hsv(green_hsv);
    BOOST_REQUIRE_CLOSE_FRACTION(green_hsv.h, deg60 * 2, .0000001f);
    BOOST_REQUIRE_CLOSE_FRACTION(green_hsv.s, 1.0f, .0000001f);
    BOOST_REQUIRE_CLOSE_FRACTION(green_hsv.v, 1.0f, .0000001f);
    BOOST_REQUIRE_EQUAL(green_rev, Colors::GREEN);
    cout << "GREEN: " << Colors::GREEN << " reved: " << green_rev << endl;

    auto blue_hsv = to_hsv(Colors::BLUE);
    auto blue_rev = from_hsv(blue_hsv);
    BOOST_REQUIRE_CLOSE_FRACTION(blue_hsv.h, deg60 * 4, .0000001f);
    BOOST_REQUIRE_CLOSE_FRACTION(blue_hsv.s, 1.0f, .0000001f);
    BOOST_REQUIRE_CLOSE_FRACTION(blue_hsv.v, 1.0f, .0000001f);
    BOOST_REQUIRE_EQUAL(blue_rev, Colors::BLUE);
    cout << "BLUE : " << Colors::BLUE << " reved: " << blue_rev << endl;

//    color_t custom(0xc1, 0xa5, 0x7d);
//    auto custom_hsv = to_hsv(custom);
//    auto custom_rev = from_hsv(custom_hsv);
//    BOOST_REQUIRE_EQUAL(custom, custom_rev);
//    cout << "CUST : " << custom << " reved: " << custom_rev << endl;

}

BOOST_AUTO_TEST_CASE(alpha)
{
	color_t base(200, 200, 200, 255);
	color_t a1(255, 0, 0, 0);

	auto c1 = base * a1;
	BOOST_REQUIRE_EQUAL(c1.r, 200);
	BOOST_REQUIRE_EQUAL(c1.a, 255);

	color_t a2(255, 0, 0, 255);
	auto c2 = base * a2;
	BOOST_REQUIRE_EQUAL(c2.r, 255);
	BOOST_REQUIRE_EQUAL(c2.a, 255);
}

BOOST_AUTO_TEST_CASE(gradient)
{
	vector<color_t> cs {Colors::WHITE, Colors::BLACK};
	ColorGradient cg(cs);
	BOOST_REQUIRE_EQUAL(Colors::WHITE, cg.clamp(.0));
	BOOST_REQUIRE_EQUAL(Colors::BLACK, cg.clamp(1));
	BOOST_REQUIRE_EQUAL(color_t(229, 229, 229), cg.clamp(.1)); // 255 + (0 - 255) * .1 = 255 + (-26)
	BOOST_REQUIRE_EQUAL(color_t(204, 204, 204), cg.clamp(.2)); // 255 - 51
}
