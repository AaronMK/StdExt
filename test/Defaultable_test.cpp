#include <StdExt/Defaultable.h>

#include <concepts>

using namespace StdExt;

template<int default_val = 0>
using Int = DefaultableMember<int, default_val>;

template<float default_val = 0.0f>
using Float = DefaultableMember<float, default_val>;

struct DefaultableClass
{
	Int<1>      field_one;
	Int<2>      field_two;
	Float<3.3f> field_3_3;
};

void testDefaultable()
{
	// Type equivalency testing of operators for mixed template parameters.
	static_assert( std::same_as<decltype(Int<1>() +   Float<5.0f>()), decltype(1 + 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() -   Float<5.0f>()), decltype(1 - 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() *   Float<5.0f>()), decltype(1 * 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() /   Float<5.0f>()), decltype(1 / 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() <   Float<5.0f>()), decltype(1 < 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() <=  Float<5.0f>()), decltype(1 <= 5.0f)>  );
	static_assert( std::same_as<decltype(Int<1>() ==  Float<5.0f>()), decltype(1 == 5.0f)>  );
	static_assert( std::same_as<decltype(Int<1>() !=  Float<5.0f>()), decltype(1 != 5.0f)>  );
	static_assert( std::same_as<decltype(Int<1>() >=  Float<5.0f>()), decltype(1 >= 5.0f)>  );
	static_assert( std::same_as<decltype(Int<1>() >   Float<5.0f>()), decltype(1 > 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() <=> Float<5.0f>()), decltype(1 <=> 5.0f)> );

	// Type equivalency testing of operators for left fundamental parameters.
	static_assert( std::same_as<decltype(1 +   Float<5.0f>()), decltype(1 + 5.0f)>   );
	static_assert( std::same_as<decltype(1 -   Float<5.0f>()), decltype(1 - 5.0f)>   );
	static_assert( std::same_as<decltype(1 *   Float<5.0f>()), decltype(1 * 5.0f)>   );
	static_assert( std::same_as<decltype(1 /   Float<5.0f>()), decltype(1 / 5.0f)>   );
	static_assert( std::same_as<decltype(1 <   Float<5.0f>()), decltype(1 < 5.0f)>   );
	static_assert( std::same_as<decltype(1 <=  Float<5.0f>()), decltype(1 <= 5.0f)>  );
	static_assert( std::same_as<decltype(1 ==  Float<5.0f>()), decltype(1 == 5.0f)>  );
	static_assert( std::same_as<decltype(1 !=  Float<5.0f>()), decltype(1 != 5.0f)>  );
	static_assert( std::same_as<decltype(1 >=  Float<5.0f>()), decltype(1 >= 5.0f)>  );
	static_assert( std::same_as<decltype(1 >   Float<5.0f>()), decltype(1 > 5.0f)>   );
	static_assert( std::same_as<decltype(1 <=> Float<5.0f>()), decltype(1 <=> 5.0f)> );
	
	// Type equivalency testing of operators for right fundamental parameters.
	static_assert( std::same_as<decltype(Int<1>() +   5.0f), decltype(1 + 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() -   5.0f), decltype(1 - 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() *   5.0f), decltype(1 * 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() /   5.0f), decltype(1 / 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() <   5.0f), decltype(1 < 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() <=  5.0f), decltype(1 <= 5.0f)>  );
	static_assert( std::same_as<decltype(Int<1>() ==  5.0f), decltype(1 == 5.0f)>  );
	static_assert( std::same_as<decltype(Int<1>() !=  5.0f), decltype(1 != 5.0f)>  );
	static_assert( std::same_as<decltype(Int<1>() >=  5.0f), decltype(1 >= 5.0f)>  );
	static_assert( std::same_as<decltype(Int<1>() >   5.0f), decltype(1 > 5.0f)>   );
	static_assert( std::same_as<decltype(Int<1>() <=> 5.0f), decltype(1 <=> 5.0f)> );

	
	// Result equivalency testing of operators for mixed template parameters.
	static_assert( (Int<1>() +   Float<5.0f>()) == (1 + 5.0f)   );
	static_assert( (Int<1>() -   Float<5.0f>()) == (1 - 5.0f)   );
	static_assert( (Int<1>() *   Float<5.0f>()) == (1 * 5.0f)   );
	static_assert( (Int<1>() /   Float<5.0f>()) == (1 / 5.0f)   );
	static_assert( (Int<1>() <   Float<5.0f>()) == (1 < 5.0f)   );
	static_assert( (Int<1>() <=  Float<5.0f>()) == (1 <= 5.0f)  );
	static_assert( (Int<1>() ==  Float<5.0f>()) == (1 == 5.0f)  );
	static_assert( (Int<1>() !=  Float<5.0f>()) == (1 != 5.0f)  );
	static_assert( (Int<1>() >=  Float<5.0f>()) == (1 >= 5.0f)  );
	static_assert( (Int<1>() >   Float<5.0f>()) == (1 > 5.0f)   );
	static_assert( (Int<1>() <=> Float<5.0f>()) == (1 <=> 5.0f) );

	// Result equivalency testing of operators for left fundamental parameters.
	static_assert( (1 +   Float<5.0f>()) == (1 + 5.0f)   );
	static_assert( (1 -   Float<5.0f>()) == (1 - 5.0f)   );
	static_assert( (1 *   Float<5.0f>()) == (1 * 5.0f)   );
	static_assert( (1 /   Float<5.0f>()) == (1 / 5.0f)   );
	static_assert( (1 <   Float<5.0f>()) == (1 < 5.0f)   );
	static_assert( (1 <=  Float<5.0f>()) == (1 <= 5.0f)  );
	static_assert( (1 ==  Float<5.0f>()) == (1 == 5.0f)  );
	static_assert( (1 !=  Float<5.0f>()) == (1 != 5.0f)  );
	static_assert( (1 >=  Float<5.0f>()) == (1 >= 5.0f)  );
	static_assert( (1 >   Float<5.0f>()) == (1 > 5.0f)   );
	static_assert( (1 <=> Float<5.0f>()) == (1 <=> 5.0f) );
	
	// Result equivalency testing of operators for right fundamental parameters.
	static_assert( (Int<1>() +   5.0f) == (1 + 5.0f)   );
	static_assert( (Int<1>() -   5.0f) == (1 - 5.0f)   );
	static_assert( (Int<1>() *   5.0f) == (1 * 5.0f)   );
	static_assert( (Int<1>() /   5.0f) == (1 / 5.0f)   );
	static_assert( (Int<1>() <   5.0f) == (1 < 5.0f)   );
	static_assert( (Int<1>() <=  5.0f) == (1 <= 5.0f)  );
	static_assert( (Int<1>() ==  5.0f) == (1 == 5.0f)  );
	static_assert( (Int<1>() !=  5.0f) == (1 != 5.0f)  );
	static_assert( (Int<1>() >=  5.0f) == (1 >= 5.0f)  );
	static_assert( (Int<1>() >   5.0f) == (1 > 5.0f)   );
	static_assert( (Int<1>() <=> 5.0f) == (1 <=> 5.0f) );

	DefaultableClass dc;
	dc.field_one = 5;
	dc.field_two = 7;

	auto add_test = dc.field_one + dc.field_3_3;

	DefaultableClass dc_next(std::move(dc));
	dc.field_one = dc.field_two;
	dc.field_3_3 = 1.4;

	dc_next.field_one = std::move(dc.field_3_3);

	auto inc_test = ++dc.field_one;
}