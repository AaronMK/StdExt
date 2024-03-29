#include <StdExt/TemplateUtility.h>

using namespace StdExt;

#include <tuple>
#include <string>
#include <iostream>

void testTemplateUtility()
{
	static_assert( std::is_same_v<
		std::tuple<int, float>,
		Types<int, float>::apply<std::tuple>
	> );

	static_assert( std::is_same_v<
		Types<int, float, double, int>,
		Types<int, float>::append<double, int>
	> );

	static_assert( std::is_same_v<
		Types<int, float, double, int>,
		Types<double, int>::prepend<int, float>
	> );

	static_assert( std::is_same_v<
		Types<int, float, double, int>,
		decltype(Types<int, float>() + Types<double, int>())
	> );
	
	static_assert( std::is_same_v<
		Types<int, float, double>,
		Types<int, float, double, std::string, bool>::first_n<3>
	> );

	static_assert( std::is_same_v<
		Types<double, std::string, bool>,
		Types<int, float, double, std::string, bool>::last_n<3>
	> );

	static_assert( std::is_same_v<
		Types<>,
		Types<int, float, double, std::string, bool>::last_n<0>
	> );
}