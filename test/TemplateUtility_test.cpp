#include <StdExt/TemplateUtility.h>

using namespace StdExt;

#include <tuple>

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
}