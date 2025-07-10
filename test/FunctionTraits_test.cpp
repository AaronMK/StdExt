#include <StdExt/FunctionTraits.h>
#include <StdExt/Type.h>

#include <stdexcept>
#include <string>
#include <tuple>

static int FreeExcept(float i)
{
	if (i < 0)
		throw std::invalid_argument("i < 0");
	
	return i + 2;
}

static float FreeNoExcept(int i) noexcept
{
	return i + 2;
}

struct StaticExceptTest
{
	static int Except(float i)
	{
		if (i < 0)
			throw std::invalid_argument("i < 0");
		
		return i + 2;
	}

	static float NoExcept(int i) noexcept
	{
		return i + 2;
	}
};

struct NonStatic
{
	int Except(float i)
	{
		if (i < 0.0f)
			throw std::invalid_argument("i < 0");
		
		return i + 2;
	}

	float NoExcept(int i) noexcept
	{
		return i + 2;
	}

	int ConstExcept(float i) const
	{
		if (i < 0.0f)
			throw std::invalid_argument("i < 0");
		
		return i + 2;
	}

	float ConstNoExcept(int i) const noexcept
	{
		return i + 2;
	}
};

struct ParameterAmbigious
{
	std::string last;

	float Func(const std::string& str, int i)
	{
		last = str;
		return static_cast<float>(i);
	}

	float Func(int i, const std::string& str)
	{
		last = str;
		return static_cast<float>(i);
	}
};

class PureVirtual
{
public:
	virtual int AddExcept(float f) = 0;
	virtual float AddNoExcept(int i) noexcept = 0;

	virtual int AddConstExcept(float f) const = 0;
	virtual float AddConstNoExcept(int i) const noexcept = 0;
};

class ImplementedPureVirtual : public PureVirtual
{
public:
	int AddExcept(float f) override
	{
		if (f < 0.0f)
			throw std::invalid_argument("i < 0");
		
		return f + 2;
	}

	float AddNoExcept(int i) noexcept override
	{
		return i + 2;
	}

	int AddConstExcept(float f) const override
	{
		if (f < 0.0f)
			throw std::invalid_argument("i < 0");
		
		return f + 2;
	}

	float AddConstNoExcept(int i) const noexcept override
	{
		return i + 2;
	}
};

void testFunctionTraits()
{
	using namespace StdExt;
	constexpr StaticExceptTest static_except_test_obj;

	static_assert( false == FunctionPointer<int> );
	static_assert( false == FunctionPointer<std::string> );
	static_assert( false == FunctionPointer<decltype(&static_except_test_obj)> );
	static_assert( true  == FunctionPointer<decltype(&FreeExcept)> );
	static_assert( true  == FunctionPointer<decltype(&FreeNoExcept)> );
	static_assert( true  == FunctionPointer<decltype(&StaticExceptTest::Except)> );
	static_assert( true  == FunctionPointer<decltype(&StaticExceptTest::NoExcept)> );
	static_assert( true  == FunctionPointer<decltype(&NonStatic::Except)> );
	static_assert( true  == FunctionPointer<decltype(&NonStatic::NoExcept)> );
	static_assert( true  == FunctionPointer<decltype(&NonStatic::ConstExcept)> );
	static_assert( true  == FunctionPointer<decltype(&NonStatic::ConstNoExcept)> );
	static_assert( true  == FunctionPointer<decltype(&PureVirtual::AddExcept)> );
	static_assert( true  == FunctionPointer<decltype(&PureVirtual::AddNoExcept)> );
	static_assert( true  == FunctionPointer<decltype(&PureVirtual::AddConstExcept)> );
	static_assert( true  == FunctionPointer<decltype(&PureVirtual::AddConstNoExcept)> );

	static_assert( false == StaticFunctionPointer<int> );
	static_assert( false == StaticFunctionPointer<std::string> );
	static_assert( false == StaticFunctionPointer<decltype(&static_except_test_obj)> );
	static_assert( true  == StaticFunctionPointer<decltype(&FreeExcept)> );
	static_assert( true  == StaticFunctionPointer<decltype(&FreeNoExcept)> );
	static_assert( true  == StaticFunctionPointer<decltype(&StaticExceptTest::Except)> );
	static_assert( true  == StaticFunctionPointer<decltype(&StaticExceptTest::NoExcept)> );
	static_assert( false == StaticFunctionPointer<decltype(&NonStatic::Except)> );
	static_assert( false == StaticFunctionPointer<decltype(&NonStatic::NoExcept)> );
	static_assert( false == StaticFunctionPointer<decltype(&NonStatic::ConstExcept)> );
	static_assert( false == StaticFunctionPointer<decltype(&NonStatic::ConstNoExcept)> );
	static_assert( false == StaticFunctionPointer<decltype(&PureVirtual::AddExcept)> );
	static_assert( false == StaticFunctionPointer<decltype(&PureVirtual::AddNoExcept)> );
	static_assert( false == StaticFunctionPointer<decltype(&PureVirtual::AddConstExcept)> );
	static_assert( false == StaticFunctionPointer<decltype(&PureVirtual::AddConstNoExcept)> );

	static_assert( false == MemberFunctionPointer<int> );
	static_assert( false == MemberFunctionPointer<std::string> );
	static_assert( false == MemberFunctionPointer<decltype(&static_except_test_obj)> );
	static_assert( false == MemberFunctionPointer<decltype(&FreeExcept)> );
	static_assert( false == MemberFunctionPointer<decltype(&FreeNoExcept)> );
	static_assert( false == MemberFunctionPointer<decltype(&StaticExceptTest::Except)> );
	static_assert( false == MemberFunctionPointer<decltype(&StaticExceptTest::NoExcept)> );
	static_assert( true  == MemberFunctionPointer<decltype(&NonStatic::Except)> );
	static_assert( true  == MemberFunctionPointer<decltype(&NonStatic::NoExcept)> );
	static_assert( true  == MemberFunctionPointer<decltype(&NonStatic::ConstExcept)> );
	static_assert( true  == MemberFunctionPointer<decltype(&NonStatic::ConstNoExcept)> );
	static_assert( true  == MemberFunctionPointer<decltype(&PureVirtual::AddExcept)> );
	static_assert( true  == MemberFunctionPointer<decltype(&PureVirtual::AddNoExcept)> );
	static_assert( true  == MemberFunctionPointer<decltype(&PureVirtual::AddConstExcept)> );
	static_assert( true  == MemberFunctionPointer<decltype(&PureVirtual::AddConstNoExcept)> );

	static_assert( false == NonConstFunctionPointer<int> );
	static_assert( false == NonConstFunctionPointer<std::string> );
	static_assert( false == NonConstFunctionPointer<decltype(&static_except_test_obj)> );
	static_assert( false == NonConstFunctionPointer<decltype(&FreeExcept)> );
	static_assert( false == NonConstFunctionPointer<decltype(&FreeNoExcept)> );
	static_assert( false == NonConstFunctionPointer<decltype(&StaticExceptTest::Except)> );
	static_assert( false == NonConstFunctionPointer<decltype(&StaticExceptTest::NoExcept)> );
	static_assert( true  == NonConstFunctionPointer<decltype(&NonStatic::Except)> );
	static_assert( true  == NonConstFunctionPointer<decltype(&NonStatic::NoExcept)> );
	static_assert( false == NonConstFunctionPointer<decltype(&NonStatic::ConstExcept)> );
	static_assert( false == NonConstFunctionPointer<decltype(&NonStatic::ConstNoExcept)> );
	static_assert( true  == NonConstFunctionPointer<decltype(&PureVirtual::AddExcept)> );
	static_assert( true  == NonConstFunctionPointer<decltype(&PureVirtual::AddNoExcept)> );
	static_assert( false == NonConstFunctionPointer<decltype(&PureVirtual::AddConstExcept)> );
	static_assert( false == NonConstFunctionPointer<decltype(&PureVirtual::AddConstNoExcept)> );

	static_assert( false == ConstFunctionPointer<int> );
	static_assert( false == ConstFunctionPointer<std::string> );
	static_assert( false == ConstFunctionPointer<decltype(&static_except_test_obj)> );
	static_assert( false == ConstFunctionPointer<decltype(&FreeExcept)> );
	static_assert( false == ConstFunctionPointer<decltype(&FreeNoExcept)> );
	static_assert( false == ConstFunctionPointer<decltype(&StaticExceptTest::Except)> );
	static_assert( false == ConstFunctionPointer<decltype(&StaticExceptTest::NoExcept)> );
	static_assert( false == ConstFunctionPointer<decltype(&NonStatic::Except)> );
	static_assert( false == ConstFunctionPointer<decltype(&NonStatic::NoExcept)> );
	static_assert( true  == ConstFunctionPointer<decltype(&NonStatic::ConstExcept)> );
	static_assert( true  == ConstFunctionPointer<decltype(&NonStatic::ConstNoExcept)> );
	static_assert( false == ConstFunctionPointer<decltype(&PureVirtual::AddExcept)> );
	static_assert( false == ConstFunctionPointer<decltype(&PureVirtual::AddNoExcept)> );
	static_assert( true  == ConstFunctionPointer<decltype(&PureVirtual::AddConstExcept)> );

	static_assert( false == Function<&FreeExcept>::is_noexcept );
	static_assert( false == Function<&FreeExcept>::is_const    );
	static_assert( false == Function<&FreeExcept>::is_member   );
	static_assert( 1     == Function<&FreeExcept>::arg_count   );
	static_assert( std::same_as<int,   Function<&FreeExcept>::return_t>     );
	static_assert( std::same_as<float, Function<&FreeExcept>::nth_arg_t<0>> );
	static_assert( std::same_as<void,  Function<&FreeExcept>::class_type>   );
	static_assert( std::same_as<void,  Function<&FreeExcept>::target_type>  );
	static_assert( &FreeExcept == Function<&FreeExcept>::raw_ptr );
	static_assert( std::same_as<decltype(&FreeExcept), Function<&FreeExcept>::raw_ptr_t> );

	static_assert( true  == Function<&FreeNoExcept>::is_noexcept );
	static_assert( false == Function<&FreeNoExcept>::is_const    );
	static_assert( false == Function<&FreeNoExcept>::is_member   );
	static_assert( 1     == Function<&FreeNoExcept>::arg_count   );
	static_assert( std::same_as<float, Function<&FreeNoExcept>::return_t>     );
	static_assert( std::same_as<int,   Function<&FreeNoExcept>::nth_arg_t<0>> );
	static_assert( std::same_as<void,  Function<&FreeNoExcept>::class_type>   );
	static_assert( std::same_as<void,  Function<&FreeNoExcept>::target_type>  );
	static_assert( &FreeNoExcept == Function<&FreeNoExcept>::raw_ptr );
	static_assert( std::same_as<decltype(&FreeNoExcept), Function<&FreeNoExcept>::raw_ptr_t> );

	static_assert( true  == Function<&StaticExceptTest::NoExcept>::is_noexcept );
	static_assert( false == Function<&StaticExceptTest::NoExcept>::is_const    );
	static_assert( false == Function<&StaticExceptTest::NoExcept>::is_member   );
	static_assert( 1     == Function<&StaticExceptTest::NoExcept>::arg_count   );
	static_assert( std::same_as<float, Function<&StaticExceptTest::NoExcept>::return_t>     );
	static_assert( std::same_as<int,   Function<&StaticExceptTest::NoExcept>::nth_arg_t<0>> );
	static_assert( std::same_as<void,  Function<&StaticExceptTest::NoExcept>::class_type>   );
	static_assert( std::same_as<void,  Function<&StaticExceptTest::NoExcept>::target_type>  );
	static_assert( &StaticExceptTest::NoExcept == Function<&StaticExceptTest::NoExcept>::raw_ptr );
	static_assert( std::same_as<decltype(&StaticExceptTest::NoExcept), Function<&StaticExceptTest::NoExcept>::raw_ptr_t> );

	static_assert( false == Function<&StaticExceptTest::Except>::is_noexcept );
	static_assert( false == Function<&StaticExceptTest::Except>::is_const    );
	static_assert( false == Function<&StaticExceptTest::Except>::is_member   );
	static_assert( 1     == Function<&StaticExceptTest::Except>::arg_count   );
	static_assert( std::same_as<int,   Function<&StaticExceptTest::Except>::return_t>     );
	static_assert( std::same_as<float, Function<&StaticExceptTest::Except>::nth_arg_t<0>> );
	static_assert( std::same_as<void,  Function<&StaticExceptTest::Except>::class_type>   );
	static_assert( std::same_as<void,  Function<&StaticExceptTest::Except>::target_type>  );
	static_assert( &StaticExceptTest::Except == Function<&StaticExceptTest::Except>::raw_ptr );
	static_assert( std::same_as<decltype(&StaticExceptTest::Except), Function<&StaticExceptTest::Except>::raw_ptr_t> );

	static_assert( false == Function<&NonStatic::Except>::is_noexcept );
	static_assert( false == Function<&NonStatic::Except>::is_const    );
	static_assert( true  == Function<&NonStatic::Except>::is_member   );
	static_assert( 1     == Function<&NonStatic::Except>::arg_count   );
	static_assert( std::same_as<int,        Function<&NonStatic::Except>::return_t>     );
	static_assert( std::same_as<float,      Function<&NonStatic::Except>::nth_arg_t<0>> );
	static_assert( std::same_as<NonStatic,  Function<&NonStatic::Except>::class_type>   );
	static_assert( std::same_as<NonStatic*, Function<&NonStatic::Except>::target_type>  );
	static_assert( &NonStatic::Except == Function<&NonStatic::Except>::raw_ptr );
	static_assert( std::same_as<decltype(&NonStatic::Except), Function<&NonStatic::Except>::raw_ptr_t> );

	static_assert( true  == Function<&NonStatic::NoExcept>::is_noexcept );
	static_assert( false == Function<&NonStatic::NoExcept>::is_const    );
	static_assert( true  == Function<&NonStatic::NoExcept>::is_member   );
	static_assert( 1     == Function<&NonStatic::NoExcept>::arg_count   );
	static_assert( std::same_as<float,      Function<&NonStatic::NoExcept>::return_t>     );
	static_assert( std::same_as<int,        Function<&NonStatic::NoExcept>::nth_arg_t<0>> );
	static_assert( std::same_as<NonStatic,  Function<&NonStatic::NoExcept>::class_type>   );
	static_assert( std::same_as<NonStatic*, Function<&NonStatic::NoExcept>::target_type>  );
	static_assert( &NonStatic::NoExcept == Function<&NonStatic::NoExcept>::raw_ptr );
	static_assert( std::same_as<decltype(&NonStatic::NoExcept), Function<&NonStatic::NoExcept>::raw_ptr_t> );

	static_assert( false == Function<&NonStatic::ConstExcept>::is_noexcept );
	static_assert( true  == Function<&NonStatic::ConstExcept>::is_const    );
	static_assert( true  == Function<&NonStatic::ConstExcept>::is_member   );
	static_assert( 1     == Function<&NonStatic::ConstExcept>::arg_count   );
	static_assert( std::same_as<int,              Function<&NonStatic::ConstExcept>::return_t>     );
	static_assert( std::same_as<float,            Function<&NonStatic::ConstExcept>::nth_arg_t<0>> );
	static_assert( std::same_as<NonStatic,        Function<&NonStatic::ConstExcept>::class_type>   );
	static_assert( std::same_as<const NonStatic*, Function<&NonStatic::ConstExcept>::target_type>  );
	static_assert( &NonStatic::ConstExcept == Function<&NonStatic::ConstExcept>::raw_ptr );
	static_assert( std::same_as<decltype(&NonStatic::ConstExcept), Function<&NonStatic::ConstExcept>::raw_ptr_t> );

	static_assert( true == Function<&NonStatic::ConstNoExcept>::is_noexcept );
	static_assert( true == Function<&NonStatic::ConstNoExcept>::is_const    );
	static_assert( true == Function<&NonStatic::ConstNoExcept>::is_member   );
	static_assert( 1    == Function<&NonStatic::ConstNoExcept>::arg_count   );
	static_assert( std::same_as<float,            Function<&NonStatic::ConstNoExcept>::return_t>     );
	static_assert( std::same_as<int,              Function<&NonStatic::ConstNoExcept>::nth_arg_t<0>> );
	static_assert( std::same_as<NonStatic,        Function<&NonStatic::ConstNoExcept>::class_type>   );
	static_assert( std::same_as<const NonStatic*, Function<&NonStatic::ConstNoExcept>::target_type>  );
	static_assert( &NonStatic::ConstNoExcept == Function<&NonStatic::ConstNoExcept>::raw_ptr );
	static_assert( std::same_as<decltype(&NonStatic::ConstNoExcept), Function<&NonStatic::ConstNoExcept>::raw_ptr_t> );
}