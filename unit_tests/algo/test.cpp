#define BOOST_TEST_MODULE test_algo
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <spes/algo.h>
#include <vector>
using namespace std;
using namespace spes::math;
using namespace spes::algo;

BOOST_AUTO_TEST_CASE(test_qsort)
{
    int arr[] = {1, 3, 8, 6, 5, 0, 7, 3, 2};
    function<bool(const int&, const int&)> foo = [](const int& a, const int& b) {return a < b; };
    qsort((int*)arr, 9, foo);

    BOOST_REQUIRE_EQUAL(arr[0], 0);
    BOOST_REQUIRE_EQUAL(arr[8], 8);
}


BOOST_AUTO_TEST_CASE(test_cohen_sutherland)
{
    rect rect(10, 10, 90, 90);
    line2d l1({ 20.f, 20.f }, { 80.f, 80.f });
    line2d l2({ 0.f, 0.f }, { 0.f, 100.f }); // REJECT
    line2d l3({ 1.f, 1.f }, { 100.f, 100.f });
    line2d l4({ 20.f, 1.f }, { 20.f, 100.f });
    line2d l5({ 1.f, 20.f }, { 100.f, 20.f });

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

BOOST_AUTO_TEST_CASE(test_sutherland_hodgman)
{
    polygon2d poly({});
    vector<polygon2d> out;

    rect rc(0, 0, 10, 10);

    poly.init({{100, 100}, {100, 200}, {200, 200}});
    sutherland_hodgman(poly, rc, out);
    BOOST_REQUIRE_EQUAL(out.size(), 0);

    poly.init({{0, 0}, {0, 10}, {10, 10}, {10, 0}});
    sutherland_hodgman(poly, rc, out);
    BOOST_REQUIRE_EQUAL(out.size(), 1);
    BOOST_REQUIRE_EQUAL(out[0]._num, 4);

    out.clear();
    poly.init({{-5, 5}, {5, 15}, {15, 5}, {5, -5}});
    sutherland_hodgman(poly, rc, out);
    BOOST_REQUIRE_EQUAL(out.size(), 1);
    BOOST_REQUIRE_EQUAL(out[0]._num, 4); // just same as rc

    out.clear();
    poly.init({{-2, 5}, {5, 12}, {12, 5}, {5, -2}});
    sutherland_hodgman(poly, rc, out);
    BOOST_REQUIRE_EQUAL(out.size(), 1);
    BOOST_REQUIRE_EQUAL(out[0]._num, 8);


    out.clear();
    poly.init({{2, -10}, {2, 6}, {5, -2}, {8, 6}, {8, -10}});
    sutherland_hodgman(poly, rc, out);
    BOOST_REQUIRE_EQUAL(out.size(), 2);
    BOOST_REQUIRE_EQUAL(out[0]._num, 3);
    BOOST_REQUIRE_EQUAL(out[1]._num, 3);

    rect rc2(160.00, 120.00, 640.00, 480.00);

    out.clear();
    poly.init({{381, 519}, {409, 436}, {422, 498}, {443, 431}, {467, 497}, {488, 432}, {500, 526}});
    sutherland_hodgman(poly, rc2, out);
    BOOST_REQUIRE_EQUAL(out.size(), 3);
    BOOST_REQUIRE_EQUAL(out[0]._num, 3);
    BOOST_REQUIRE_EQUAL(out[1]._num, 3);
    BOOST_REQUIRE_EQUAL(out[2]._num, 3);
}