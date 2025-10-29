#include <StdExt/Defaultable.h>
#include <StdExt/Test/Test.h>

#include <concepts>

using namespace StdExt;

template<int32_t default_val = 0>
using Int = Defaultable<int32_t, default_val>;

template<uint16_t default_val = 0>
using Uint = Defaultable<uint16_t, default_val>;

template<float default_val = 0.0f>
using Float = Defaultable<float, default_val>;

struct DefaultableClass
{
	Int<1>      field_one;
	Int<2>      field_two;
	Uint<33>    field_33;
	Float<5.5f> field_55;
};

template<typename left_t, typename right_t>
static void testAssignment(const left_t& left_val, const right_t& right_val)
{
	using LeftDef_t = Defaultable<left_t>;
	using RightDef_t = Defaultable<right_t>;

	left_t  left_var  = left_val;
	left_t  left_copy = left_val;

	right_t right_var  = right_val;
	right_t right_copy = right_val;

	LeftDef_t  LeftDef;
	RightDef_t RightDef;

	auto reset = [&]()
	{
		left_var = left_val;
		left_copy = left_val;
		LeftDef     = left_val;

		right_var  = right_val;
		right_copy = right_val;
		RightDef   = right_val;
	};

	// Type equivalency testing of operators for mixed template parameters.
	static_assert( std::same_as<decltype(LeftDef +=  RightDef_t()), decltype(left_var +=  right_val)> );
	static_assert( std::same_as<decltype(LeftDef -=  RightDef_t()), decltype(left_var -=  right_val)> );
	static_assert( std::same_as<decltype(LeftDef *=  RightDef_t()), decltype(left_var *=  right_val)> );
	static_assert( std::same_as<decltype(LeftDef /=  RightDef_t()), decltype(left_var /=  right_val)> );
	static_assert( std::same_as<decltype(LeftDef %=  RightDef_t()), decltype(left_var %=  right_val)> );
	static_assert( std::same_as<decltype(LeftDef &=  RightDef_t()), decltype(left_var &=  right_val)> );
	static_assert( std::same_as<decltype(LeftDef |=  RightDef_t()), decltype(left_var |=  right_val)> );
	static_assert( std::same_as<decltype(LeftDef ^=  RightDef_t()), decltype(left_var ^=  right_val)> );
	static_assert( std::same_as<decltype(LeftDef <<= RightDef_t()), decltype(left_var <<= right_val)> );
	static_assert( std::same_as<decltype(LeftDef >>= RightDef_t()), decltype(left_var >>= right_val)> );

	// Type equivalency testing of operators for left fundamental parameters.
	static_assert( std::same_as<decltype(left_var +=  RightDef_t()), decltype(left_var +=  right_var)> );
	static_assert( std::same_as<decltype(left_var -=  RightDef_t()), decltype(left_var -=  right_var)> );
	static_assert( std::same_as<decltype(left_var *=  RightDef_t()), decltype(left_var *=  right_var)> );
	static_assert( std::same_as<decltype(left_var /=  RightDef_t()), decltype(left_var /=  right_var)> );
	static_assert( std::same_as<decltype(left_var %=  RightDef_t()), decltype(left_var %=  right_var)> );
	static_assert( std::same_as<decltype(left_var &=  RightDef_t()), decltype(left_var &=  right_var)> );
	static_assert( std::same_as<decltype(left_var |=  RightDef_t()), decltype(left_var |=  right_var)> );
	static_assert( std::same_as<decltype(left_var ^=  RightDef_t()), decltype(left_var ^=  right_var)> );
	static_assert( std::same_as<decltype(left_var <<= RightDef_t()), decltype(left_var <<= right_var)> );
	static_assert( std::same_as<decltype(left_var >>= RightDef_t()), decltype(left_var >>= right_var)> );

	// Type equivalency testing of operators for right fundamental parameters.
	static_assert( std::same_as<decltype(LeftDef +=  right_var), decltype(left_var +=  right_var)> );
	static_assert( std::same_as<decltype(LeftDef -=  right_var), decltype(left_var -=  right_var)> );
	static_assert( std::same_as<decltype(LeftDef *=  right_var), decltype(left_var *=  right_var)> );
	static_assert( std::same_as<decltype(LeftDef /=  right_var), decltype(left_var /=  right_var)> );
	static_assert( std::same_as<decltype(LeftDef %=  right_var), decltype(left_var %=  right_var)> );
	static_assert( std::same_as<decltype(LeftDef &=  right_var), decltype(left_var &=  right_var)> );
	static_assert( std::same_as<decltype(LeftDef |=  right_var), decltype(left_var |=  right_var)> );
	static_assert( std::same_as<decltype(LeftDef ^=  right_var), decltype(left_var ^=  right_var)> );
	static_assert( std::same_as<decltype(LeftDef <<= right_var), decltype(left_var <<= right_var)> );
	static_assert( std::same_as<decltype(LeftDef >>= right_var), decltype(left_var >>= right_var)> );

	// Result equivalency testing of operators for mixed template parameters.
	{
		reset();

		auto check_and_reset = [&](const std::string& msg)
		{
			Test::testForResult<bool>(
				"Defaultable Assignable Left and Right - " + msg,
				LeftDef.Value == left_copy && RightDef.Value == right_copy, true
			);

			reset();
		};

		LeftDef +=  RightDef; left_copy +=  right_copy; check_and_reset("+=");
		LeftDef -=  RightDef; left_copy -=  right_copy; check_and_reset("-=");
		LeftDef *=  RightDef; left_copy *=  right_copy; check_and_reset("*=");
		LeftDef /=  RightDef; left_copy /=  right_copy; check_and_reset("/=");
		LeftDef %=  RightDef; left_copy %=  right_copy; check_and_reset("%=");
		LeftDef &=  RightDef; left_copy &=  right_copy; check_and_reset("&=");
		LeftDef |=  RightDef; left_copy |=  right_copy; check_and_reset("|=");
		LeftDef ^=  RightDef; left_copy ^=  right_copy; check_and_reset("^=");
		LeftDef <<= RightDef; left_copy <<= right_copy; check_and_reset("<<=");
		LeftDef >>= RightDef; left_copy >>= right_copy; check_and_reset(">>=");
	}

	{
		reset();

		auto check_and_reset = [&](const std::string& msg)
		{
			Test::testForResult<bool>(
				"Defaultable Assignable Right - " + msg,
				left_var == left_copy && RightDef.Value == right_copy, true
			);

			reset();
		};

		// Result equivalency testing of operators for left fundamental parameters.
		left_var +=  RightDef; left_copy +=  right_copy; check_and_reset("+=");
		left_var -=  RightDef; left_copy -=  right_copy; check_and_reset("-=");
		left_var *=  RightDef; left_copy *=  right_copy; check_and_reset("*=");
		left_var /=  RightDef; left_copy /=  right_copy; check_and_reset("/=");
		left_var %=  RightDef; left_copy %=  right_copy; check_and_reset("%=");
		left_var &=  RightDef; left_copy &=  right_copy; check_and_reset("&=");
		left_var |=  RightDef; left_copy |=  right_copy; check_and_reset("|=");
		left_var ^=  RightDef; left_copy ^=  right_copy; check_and_reset("^=");
		left_var <<= RightDef; left_copy <<= right_copy; check_and_reset("<<=");
		left_var >>= RightDef; left_copy >>= right_copy; check_and_reset(">>=");
	}

	{
		reset();

		auto check_and_reset = [&](const std::string& msg)
		{
			Test::testForResult<bool>(
				"Defaultable Assignable Left - " + msg,
				LeftDef.Value == left_copy && right_var == right_copy, true
			);

			reset();
		};

		// Result equivalency testing of operators for mixed template parameters.
		LeftDef +=  right_var; left_copy +=  right_copy; check_and_reset("+=");
		LeftDef -=  right_var; left_copy -=  right_copy; check_and_reset("-=");
		LeftDef *=  right_var; left_copy *=  right_copy; check_and_reset("*=");
		LeftDef /=  right_var; left_copy /=  right_copy; check_and_reset("/=");
		LeftDef %=  right_var; left_copy %=  right_copy; check_and_reset("%=");
		LeftDef &=  right_var; left_copy &=  right_copy; check_and_reset("&=");
		LeftDef |=  right_var; left_copy |=  right_copy; check_and_reset("|=");
		LeftDef ^=  right_var; left_copy ^=  right_copy; check_and_reset("^=");
		LeftDef <<= right_var; left_copy <<= right_copy; check_and_reset("<<=");
		LeftDef >>= right_var; left_copy >>= right_copy; check_and_reset(">>=");
	}

	
	if constexpr (
		PrefixIncrement<left_t>::is_valid && PostfixIncrement<left_t>::is_valid && 
		PrefixDecrement<left_t>::is_valid && PostfixDecrement<left_t>::is_valid
	)
	{
		reset();

		left_t def_result = left_t{};
		left_t scl_result = left_t{};

		auto check_and_reset = [&](const std::string& msg)
		{
			Test::testForResult<bool>(
				"Defaultable Increment/Decrement " + msg,
				LeftDef.Value == left_copy && def_result == scl_result, true
			);

			def_result = left_t{};
			scl_result = left_t{};

			LeftDef   = left_val;
			left_copy = left_val;
		};

		// Result equivalency testing of operators for mixed template parameters.
		def_result = LeftDef++; scl_result = left_copy++; check_and_reset("postfix ++");
		def_result = LeftDef--; scl_result = left_copy--; check_and_reset("postfix --");
		def_result = ++LeftDef; scl_result = ++left_copy; check_and_reset("prefix  ++");
		def_result = --LeftDef; scl_result = --left_copy; check_and_reset("prefix  --");
	}
}


template<auto left_default_val, decltype(left_default_val) left_alt_val, auto right_default_val>
void testOperators()
{
	using left_t  = decltype(left_default_val);
	using right_t = decltype(right_default_val);

	using LeftDef_t = Defaultable<left_t, left_default_val>;
	using RightDef_t = Defaultable<right_t, right_default_val>;

	// Type equivalency testing of operators for mixed template parameters.
	static_assert( std::same_as<decltype(LeftDef_t() +   RightDef_t()), decltype(left_default_val +   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() -   RightDef_t()), decltype(left_default_val -   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() *   RightDef_t()), decltype(left_default_val *   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() /   RightDef_t()), decltype(left_default_val /   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() %   RightDef_t()), decltype(left_default_val %   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() &   RightDef_t()), decltype(left_default_val &   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() |   RightDef_t()), decltype(left_default_val |   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() ^   RightDef_t()), decltype(left_default_val ^   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() <<  RightDef_t()), decltype(left_default_val <<  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() >>  RightDef_t()), decltype(left_default_val >>  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() <   RightDef_t()), decltype(left_default_val <   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() <=  RightDef_t()), decltype(left_default_val <=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() ==  RightDef_t()), decltype(left_default_val ==  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() !=  RightDef_t()), decltype(left_default_val !=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() >=  RightDef_t()), decltype(left_default_val >=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() >   RightDef_t()), decltype(left_default_val >   right_default_val)> );

	// Type equivalency testing of operators for left fundamental parameters.
	static_assert( std::same_as<decltype(left_default_val +   RightDef_t()), decltype(left_default_val +   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val -   RightDef_t()), decltype(left_default_val -   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val *   RightDef_t()), decltype(left_default_val *   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val /   RightDef_t()), decltype(left_default_val /   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val %   RightDef_t()), decltype(left_default_val %   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val &   RightDef_t()), decltype(left_default_val &   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val |   RightDef_t()), decltype(left_default_val |   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val ^   RightDef_t()), decltype(left_default_val ^   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val <<  RightDef_t()), decltype(left_default_val <<  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val >>  RightDef_t()), decltype(left_default_val >>  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val <   RightDef_t()), decltype(left_default_val <   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val <=  RightDef_t()), decltype(left_default_val <=  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val ==  RightDef_t()), decltype(left_default_val ==  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val !=  RightDef_t()), decltype(left_default_val !=  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val >=  RightDef_t()), decltype(left_default_val >=  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val >   RightDef_t()), decltype(left_default_val >   right_default_val)> );
	
	// Type equivalency testing of operators for right fundamental parameters.
	static_assert( std::same_as<decltype(LeftDef_t() +   right_default_val), decltype(left_default_val +   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() -   right_default_val), decltype(left_default_val -   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() *   right_default_val), decltype(left_default_val *   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() /   right_default_val), decltype(left_default_val /   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() %   right_default_val), decltype(left_default_val %   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() &   right_default_val), decltype(left_default_val &   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() |   right_default_val), decltype(left_default_val |   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() ^   right_default_val), decltype(left_default_val ^   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() <<  right_default_val), decltype(left_default_val <<  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() >>  right_default_val), decltype(left_default_val >>  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() <   right_default_val), decltype(left_default_val <   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() <=  right_default_val), decltype(left_default_val <=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() ==  right_default_val), decltype(left_default_val ==  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() !=  right_default_val), decltype(left_default_val !=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() >=  right_default_val), decltype(left_default_val >=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef_t() >   right_default_val), decltype(left_default_val >   right_default_val)> );

	// Result equivalency testing of operators for mixed template parameters.
	static_assert( (LeftDef_t() +   RightDef_t()) == (left_default_val +   right_default_val) );
	static_assert( (LeftDef_t() -   RightDef_t()) == (left_default_val -   right_default_val) );
	static_assert( (LeftDef_t() *   RightDef_t()) == (left_default_val *   right_default_val) );
	static_assert( (LeftDef_t() /   RightDef_t()) == (left_default_val /   right_default_val) );
	static_assert( (LeftDef_t() %   RightDef_t()) == (left_default_val %   right_default_val) );
	static_assert( (LeftDef_t() &   RightDef_t()) == (left_default_val &   right_default_val) );
	static_assert( (LeftDef_t() |   RightDef_t()) == (left_default_val |   right_default_val) );
	static_assert( (LeftDef_t() ^   RightDef_t()) == (left_default_val ^   right_default_val) );
	static_assert( (LeftDef_t() <<  RightDef_t()) == (left_default_val <<  right_default_val) );
	static_assert( (LeftDef_t() >>  RightDef_t()) == (left_default_val >>  right_default_val) );
	static_assert( (LeftDef_t() <   RightDef_t()) == (left_default_val <   right_default_val) );
	static_assert( (LeftDef_t() <=  RightDef_t()) == (left_default_val <=  right_default_val) );
	static_assert( (LeftDef_t() ==  RightDef_t()) == (left_default_val ==  right_default_val) );
	static_assert( (LeftDef_t() !=  RightDef_t()) == (left_default_val !=  right_default_val) );
	static_assert( (LeftDef_t() >=  RightDef_t()) == (left_default_val >=  right_default_val) );
	static_assert( (LeftDef_t() >   RightDef_t()) == (left_default_val >   right_default_val) );

	// Result equivalency testing of operators for left fundamental parameters.
	static_assert( (left_default_val +   RightDef_t()) == (left_default_val +   right_default_val) );
	static_assert( (left_default_val -   RightDef_t()) == (left_default_val -   right_default_val) );
	static_assert( (left_default_val *   RightDef_t()) == (left_default_val *   right_default_val) );
	static_assert( (left_default_val /   RightDef_t()) == (left_default_val /   right_default_val) );
	static_assert( (left_default_val %   RightDef_t()) == (left_default_val %   right_default_val) );
	static_assert( (left_default_val &   RightDef_t()) == (left_default_val &   right_default_val) );
	static_assert( (left_default_val |   RightDef_t()) == (left_default_val |   right_default_val) );
	static_assert( (left_default_val ^   RightDef_t()) == (left_default_val ^   right_default_val) );
	static_assert( (left_default_val <<  RightDef_t()) == (left_default_val <<  right_default_val) );
	static_assert( (left_default_val >>  RightDef_t()) == (left_default_val >>  right_default_val) );
	static_assert( (left_default_val <   RightDef_t()) == (left_default_val <   right_default_val) );
	static_assert( (left_default_val <=  RightDef_t()) == (left_default_val <=  right_default_val) );
	static_assert( (left_default_val ==  RightDef_t()) == (left_default_val ==  right_default_val) );
	static_assert( (left_default_val !=  RightDef_t()) == (left_default_val !=  right_default_val) );
	static_assert( (left_default_val >=  RightDef_t()) == (left_default_val >=  right_default_val) );
	static_assert( (left_default_val >   RightDef_t()) == (left_default_val >   right_default_val) );
	
	// Result equivalency testing of operators for right fundamental parameters.
	static_assert( (LeftDef_t() +   right_default_val) == (left_default_val +   right_default_val) );
	static_assert( (LeftDef_t() -   right_default_val) == (left_default_val -   right_default_val) );
	static_assert( (LeftDef_t() *   right_default_val) == (left_default_val *   right_default_val) );
	static_assert( (LeftDef_t() /   right_default_val) == (left_default_val /   right_default_val) );
	static_assert( (LeftDef_t() %   right_default_val) == (left_default_val %   right_default_val) );
	static_assert( (LeftDef_t() &   right_default_val) == (left_default_val &   right_default_val) );
	static_assert( (LeftDef_t() |   right_default_val) == (left_default_val |   right_default_val) );
	static_assert( (LeftDef_t() ^   right_default_val) == (left_default_val ^   right_default_val) );
	static_assert( (LeftDef_t() <<  right_default_val) == (left_default_val <<  right_default_val) );
	static_assert( (LeftDef_t() >>  right_default_val) == (left_default_val >>  right_default_val) );
	static_assert( (LeftDef_t() <   right_default_val) == (left_default_val <   right_default_val) );
	static_assert( (LeftDef_t() <=  right_default_val) == (left_default_val <=  right_default_val) );
	static_assert( (LeftDef_t() ==  right_default_val) == (left_default_val ==  right_default_val) );
	static_assert( (LeftDef_t() !=  right_default_val) == (left_default_val !=  right_default_val) );
	static_assert( (LeftDef_t() >=  right_default_val) == (left_default_val >=  right_default_val) );
	static_assert( (LeftDef_t() >   right_default_val) == (left_default_val >   right_default_val) );

	if constexpr ( ThreeWayCompare<left_t, right_t>::is_valid )
	{
		static_assert( std::same_as<decltype(LeftDef_t() <=> RightDef_t()), decltype(left_default_val <=> right_default_val)> );
		static_assert( std::same_as<decltype(left_default_val <=> RightDef_t()), decltype(left_default_val <=> right_default_val)> );
		static_assert( std::same_as<decltype(LeftDef_t() <=> right_default_val), decltype(left_default_val <=> right_default_val)> );
		static_assert( (LeftDef_t() <=> right_default_val) == (left_default_val <=> right_default_val) );
		static_assert( (left_default_val <=> RightDef_t()) == (left_default_val <=> right_default_val) );
		static_assert( (LeftDef_t() <=> RightDef_t()) == (left_default_val <=> right_default_val) );
	}
}

void testDefaultable()
{
	using namespace StdExt::Test;

	constexpr uint64_t left_default_1  = 27; // binary 11011
	constexpr uint64_t left_default_2  = 15; // binary 11011
	constexpr int32_t  right_default_1 = 6;  // binary 00110
	testOperators<left_default_1, 75, right_default_1>();
	testAssignment<uint64_t, int32_t>(left_default_1, right_default_1);

	Defaultable<const int32_t*> defaulted_ptr(&right_default_1);
	testForResult<int32_t>("Defaultable scaler pointer dereferences to expected value.", *defaulted_ptr, right_default_1);

	const Defaultable<const int32_t*> const_defaulted_ptr(&right_default_1);
	testForResult<int32_t>("const Defaultable scaler pointer dereferences to expected value.", *const_defaulted_ptr, right_default_1);

	std::string str_1("str 1");
	Defaultable<std::string*> defaulted_str_ptr(&str_1);
	testForResult("Defaultable scaler pointer function call operator give equivelent result to direct call.", defaulted_str_ptr->c_str(), str_1.c_str());

	const std::string const_str_1("const str 1");
	const Defaultable<const std::string*> const_defaulted_str_ptr(&const_str_1);
	testForResult("const Defaultable scaler pointer function call operator give equivelent result to direct call.", const_defaulted_str_ptr->c_str(), const_str_1.c_str());

	Float<3.0f> float_3_0;

	testForResult<float>("Defaultable value has expected initial value on default construction.", float_3_0, 3.0f);
	testForResult<float>("Defaultable value has expected default_value.", float_3_0.default_value, 3.0f);
	testForResult<float>("Defaultable value has takes the value of its constructor parameter.", Float<3.0f>(4.0f), 4.0f);

	float_3_0 = 4.0f;
	testForResult<float>("Defaultable accepts a new value.", float_3_0, 4.0f);

	Float<3.0f> copy_target(float_3_0);
	testForResult<float>("Defaultable retains its value after being the source of a copy construction.", float_3_0, 4.0f);
	testForResult<float>("Defaultable takes the value of a copied Defaultable when the target of a copy construction.", copy_target, 4.0f);

	copy_target = copy_target.default_value;
	copy_target = float_3_0;

	testForResult<float>("Defaultable retains its value after being the source of a copy assignment.", float_3_0, 4.0f);
	testForResult<float>("Defaultable takes the value of a copied Defaultable when the target of a copy assignment.", copy_target, 4.0f);

	Float<3.0f> move_target(std::move(float_3_0));
	testForResult<float>("Defaultable returns to default value after being the source of a move constructor.", float_3_0, 3.0f);
	testForResult<float>("Defaultable takes the value of a moved Defaultable when the target of a move constructor.", move_target, 4.0f);

	float_3_0 = 4.0f;
	move_target = move_target.default_value;

	move_target = std::move(float_3_0);
	testForResult<float>("Defaultable returns to default value after being the source of a move assignment.", float_3_0, 3.0f);
	testForResult<float>("Defaultable takes the value of a moved Defaultable when the target of a move assignment.", move_target, 4.0f);

	DefaultableClass DC_orig;
	DC_orig.field_33 = 22;
	DC_orig.field_55 = 1.1f;
	DC_orig.field_one = 20;
	DC_orig.field_two = 30;

	DefaultableClass DC_copy(DC_orig);

	testForResult<bool>(
		"Defaultable members of class retain values when parent object is the source of a default copy constructor.", true,
		22 == DC_orig.field_33 && 1.1f == DC_orig.field_55 && 20 == DC_orig.field_one && 30 == DC_orig.field_two
	);

	testForResult<bool>(
		"Defaultable members of class get source values when parent object is the destination of a default copy constructor.", true,
		22 == DC_copy.field_33 && 1.1f == DC_copy.field_55 && 20 == DC_copy.field_one && 30 == DC_copy.field_two
	);

	DefaultableClass DC_move(std::move(DC_orig));

	testForResult<bool>(
		"Defaultable members of class return to default values when parent object is the source of a default move constructor.", true,
		33 == DC_orig.field_33 && 5.5f == DC_orig.field_55 && 1 == DC_orig.field_one && 2 == DC_orig.field_two
	);

	testForResult<bool>(
		"Defaultable members of class get source values when parent object is the destination of a default move constructor.", true,
		22 == DC_move.field_33 && 1.1f == DC_move.field_55 && 20 == DC_move.field_one && 30 == DC_move.field_two
	);

	DC_orig.field_33 = 22;
	DC_orig.field_55 = 1.1f;
	DC_orig.field_one = 20;
	DC_orig.field_two = 30;
	DC_copy = DC_orig;

	testForResult<bool>(
		"Defaultable members of class retain values when parent object is the source of a default copy assignment.", true,
		22 == DC_orig.field_33 && 1.1f == DC_orig.field_55 && 20 == DC_orig.field_one && 30 == DC_orig.field_two
	);

	testForResult<bool>(
		"Defaultable members of class get source values when parent object is the destination of a default copy assignment.", true,
		22 == DC_copy.field_33 && 1.1f == DC_copy.field_55 && 20 == DC_copy.field_one && 30 == DC_copy.field_two
	);

	DC_move = std::move(DC_orig);

	testForResult<bool>(
		"Defaultable members of class return to default values when parent object is the source of a default move operation.", true,
		33 == DC_orig.field_33 && 5.5f == DC_orig.field_55 && 1 == DC_orig.field_one && 2 == DC_orig.field_two
	);

	testForResult<bool>(
		"Defaultable members of class get source values when parent object is the destination of a default move assignment.", true,
		22 == DC_move.field_33 && 1.1f == DC_move.field_55 && 20 == DC_move.field_one && 30 == DC_move.field_two
	);
}