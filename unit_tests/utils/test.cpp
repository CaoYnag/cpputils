#define BOOST_TEST_MODULE test_types
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <spes/utils.h>
using namespace std;

BOOST_AUTO_TEST_CASE(str_oper)
{
	BOOST_REQUIRE_EQUAL("az", str_lowercase("AZ"));
	BOOST_REQUIRE_EQUAL("AZ", str_uppercase("az"));

	BOOST_REQUIRE_EQUAL("01234567890`-=~!@#$%^&*()_+[]{}\\|:;\'\"<>,./?", str_lowercase("01234567890`-=~!@#$%^&*()_+[]{}\\|:;\'\"<>,./?"));
	BOOST_REQUIRE_EQUAL("01234567890`-=~!@#$%^&*()_+[]{}\\|:;\'\"<>,./?", str_uppercase("01234567890`-=~!@#$%^&*()_+[]{}\\|:;\'\"<>,./?"));
}
