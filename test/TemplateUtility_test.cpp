#include <StdExt/TemplateUtility.h>

using namespace StdExt;

#include <tuple>

template<size_t N, typename ...args_t>
struct first_args;

template<typename ...args_t>
struct first_args<0, args_t...>
{
	using Type = Types<>;
};

template<typename arg_t, typename ...args_tail_t>
struct first_args<1, arg_t, args_tail_t...>
{
	using Type = Types<arg_t>;
};

template<size_t N, typename arg_t, typename ...args_tail_t>
struct first_args<N, arg_t, args_tail_t...>
{
	using Type = decltype(Types<arg_t>() + typename first_args<N - 1, args_tail_t...>::Type());
};

template<size_t N, typename ...args_t>
using first_args_t = typename first_args<N, args_t...>::Type;

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

	auto indecies = first_args_t<2, int, float, bool, double>();

	auto sum = indecies + Types<std::string, bool>();
}