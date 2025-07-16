#include <StdExt/FunctionTraits.h>
#include <StdExt/Type.h>

#include <format>
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

static std::string TwoParamsFree(int i, float f) noexcept
{
	return std::format("({}, {}", i, f);
}

static void ThreeParamsFree(int i, float f, const std::string& str)
{
	if (i < 0 )
		throw std::invalid_argument("i must be less than 0");
}

class MultipleParams
{
private:
	std::string mLastString;

public:
	MultipleParams() = default;

	static std::string TwoParamsStatic(int i, float f) noexcept
	{
		return std::format("({}, {}", i, f);
	}

	static void ThreeParamsStatic(int i, float f, const std::string& str)
	{
		if (i < 0 )
			throw std::invalid_argument("i must be less than 0");
	}

	std::string TwoParamsNonConst(int i, float f) noexcept
	{
		mLastString = std::format("({}, {}", i, f);
		return mLastString;
	}

	void ThreeParamsNonConst(int i, float f, const std::string& str)
	{
		if (i < 0 )
			throw std::invalid_argument("i must be less than 0");

		mLastString = std::format("({}, {}, {}", i, f, str);
	}

	std::string TwoParamsConst(int i, float f) const noexcept
	{
		return std::format("({}, {}, {}", i, f, mLastString);
	}

	void ThreeParamsConst(int i, float f, const std::string& str) const
	{
		if (i < 0 )
			throw std::invalid_argument("i must be less than 0");
	}
};

void testFunctionTraits()
{
	using namespace StdExt;
	constexpr MultipleParams multi_params_test_obj;

	#pragma region  Concept FunctionPointer
	static_assert( false == FunctionPointer<int> );
	static_assert( false == FunctionPointer<std::string> );
	static_assert( false == FunctionPointer<decltype(&multi_params_test_obj)> );
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
	#pragma endregion

	#pragma region  Concept StaticFunctionPointer
	static_assert( false == StaticFunctionPointer<int> );
	static_assert( false == StaticFunctionPointer<std::string> );
	static_assert( false == StaticFunctionPointer<decltype(&multi_params_test_obj)> );
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
	#pragma endregion

	#pragma region  Concept MemberFunctionPointer
	static_assert( false == MemberFunctionPointer<int> );
	static_assert( false == MemberFunctionPointer<std::string> );
	static_assert( false == MemberFunctionPointer<decltype(&multi_params_test_obj)> );
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
	#pragma endregion

	#pragma region  Concept NonConstFunctionPointer
	static_assert( false == NonConstFunctionPointer<int> );
	static_assert( false == NonConstFunctionPointer<std::string> );
	static_assert( false == NonConstFunctionPointer<decltype(&multi_params_test_obj)> );
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
	#pragma endregion

	#pragma region  Concept ConstFunctionPointer
	static_assert( false == ConstFunctionPointer<int> );
	static_assert( false == ConstFunctionPointer<std::string> );
	static_assert( false == ConstFunctionPointer<decltype(&multi_params_test_obj)> );
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
	#pragma endregion

	#pragma region Function::return_type
	static_assert(std::same_as<Function<&TwoParamsFree>::return_type,                       std::string>);
	static_assert(std::same_as<Function<&ThreeParamsFree>::return_type,                     void>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::return_type,     std::string>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::return_type,   void>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::return_type,   std::string>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::return_type, void>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::return_type,      std::string>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::return_type,    void>);
	#pragma endregion

	#pragma region FunctionTraits::return_type
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::return_type,                       std::string>);
	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::return_type,                     void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::return_type,     std::string>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::return_type,   void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::return_type,   std::string>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::return_type, void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::return_type,      std::string>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::return_type,    void>);
	#pragma endregion

	#pragma region Function::arg_types
	static_assert(std::same_as<Function<&TwoParamsFree>::arg_types,                       Types<int, float>>);
	static_assert(std::same_as<Function<&ThreeParamsFree>::arg_types,                     Types<int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::arg_types,     Types<int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::arg_types,   Types<int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::arg_types,   Types<int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::arg_types, Types<int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::arg_types,      Types<int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::arg_types,    Types<int, float, const std::string&>>);
	#pragma endregion

	#pragma region FunctionTraits::arg_types
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::arg_types,                       Types<int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::arg_types,                     Types<int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::arg_types,     Types<int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::arg_types,   Types<int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::arg_types,   Types<int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::arg_types, Types<int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::arg_types,      Types<int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::arg_types,    Types<int, float, const std::string&>>);
	#pragma endregion

	#pragma region Function::arg_count
	static_assert(2 == Function<&TwoParamsFree>::arg_count);
	static_assert(3 == Function<&ThreeParamsFree>::arg_count);
	static_assert(2 == Function<&MultipleParams::TwoParamsStatic>::arg_count);
	static_assert(3 == Function<&MultipleParams::ThreeParamsStatic>::arg_count);
	static_assert(2 == Function<&MultipleParams::TwoParamsNonConst>::arg_count);
	static_assert(3 == Function<&MultipleParams::ThreeParamsNonConst>::arg_count);
	static_assert(2 == Function<&MultipleParams::TwoParamsConst>::arg_count);
	static_assert(3 == Function<&MultipleParams::ThreeParamsConst>::arg_count);
	#pragma endregion

	#pragma region FunctionTraits::arg_count
	static_assert(2 == FunctionTraits(&TwoParamsFree).arg_count);
	static_assert(3 == FunctionTraits(&ThreeParamsFree).arg_count);
	static_assert(2 == FunctionTraits(&MultipleParams::TwoParamsStatic).arg_count);
	static_assert(3 == FunctionTraits(&MultipleParams::ThreeParamsStatic).arg_count);
	static_assert(2 == FunctionTraits(&MultipleParams::TwoParamsNonConst).arg_count);
	static_assert(3 == FunctionTraits(&MultipleParams::ThreeParamsNonConst).arg_count);
	static_assert(2 == FunctionTraits(&MultipleParams::TwoParamsConst).arg_count);
	static_assert(3 == FunctionTraits(&MultipleParams::ThreeParamsConst).arg_count);
	#pragma endregion

	#pragma region Function::is_member
	static_assert(false == Function<&TwoParamsFree>::is_member);
	static_assert(false == Function<&ThreeParamsFree>::is_member);
	static_assert(false == Function<&MultipleParams::TwoParamsStatic>::is_member);
	static_assert(false == Function<&MultipleParams::ThreeParamsStatic>::is_member);
	static_assert(true  == Function<&MultipleParams::TwoParamsNonConst>::is_member);
	static_assert(true  == Function<&MultipleParams::ThreeParamsNonConst>::is_member);
	static_assert(true  == Function<&MultipleParams::TwoParamsConst>::is_member);
	static_assert(true  == Function<&MultipleParams::ThreeParamsConst>::is_member);
	#pragma endregion

	#pragma region FunctionTraits::is_member
	static_assert(false == FunctionTraits(&TwoParamsFree).is_member);
	static_assert(false == FunctionTraits(&ThreeParamsFree).is_member);
	static_assert(false == FunctionTraits(&MultipleParams::TwoParamsStatic).is_member);
	static_assert(false == FunctionTraits(&MultipleParams::ThreeParamsStatic).is_member);
	static_assert(true  == FunctionTraits(&MultipleParams::TwoParamsNonConst).is_member);
	static_assert(true  == FunctionTraits(&MultipleParams::ThreeParamsNonConst).is_member);
	static_assert(true  == FunctionTraits(&MultipleParams::TwoParamsConst).is_member);
	static_assert(true  == FunctionTraits(&MultipleParams::ThreeParamsConst).is_member);
	#pragma endregion

	#pragma region Function::is_const
	static_assert(false == Function<&TwoParamsFree>::is_const);
	static_assert(false == Function<&ThreeParamsFree>::is_const);
	static_assert(false == Function<&MultipleParams::TwoParamsStatic>::is_const);
	static_assert(false == Function<&MultipleParams::ThreeParamsStatic>::is_const);
	static_assert(false == Function<&MultipleParams::TwoParamsNonConst>::is_const);
	static_assert(false == Function<&MultipleParams::ThreeParamsNonConst>::is_const);
	static_assert(true  == Function<&MultipleParams::TwoParamsConst>::is_const);
	static_assert(true  == Function<&MultipleParams::ThreeParamsConst>::is_const);
	#pragma endregion

	#pragma region FunctionTraits::is_const
	static_assert(false == FunctionTraits(&TwoParamsFree).is_const);
	static_assert(false == FunctionTraits(&ThreeParamsFree).is_const);
	static_assert(false == FunctionTraits(&MultipleParams::TwoParamsStatic).is_const);
	static_assert(false == FunctionTraits(&MultipleParams::ThreeParamsStatic).is_const);
	static_assert(false == FunctionTraits(&MultipleParams::TwoParamsNonConst).is_const);
	static_assert(false == FunctionTraits(&MultipleParams::ThreeParamsNonConst).is_const);
	static_assert(true  == FunctionTraits(&MultipleParams::TwoParamsConst).is_const);
	static_assert(true  == FunctionTraits(&MultipleParams::ThreeParamsConst).is_const);
	#pragma endregion

	#pragma region Function::is_noexcept
	static_assert(true  == Function<&TwoParamsFree>::is_noexcept);
	static_assert(false == Function<&ThreeParamsFree>::is_noexcept);
	static_assert(true  == Function<&MultipleParams::TwoParamsStatic>::is_noexcept);
	static_assert(false == Function<&MultipleParams::ThreeParamsStatic>::is_noexcept);
	static_assert(true  == Function<&MultipleParams::TwoParamsNonConst>::is_noexcept);
	static_assert(false == Function<&MultipleParams::ThreeParamsNonConst>::is_noexcept);
	static_assert(true  == Function<&MultipleParams::TwoParamsConst>::is_noexcept);
	static_assert(false == Function<&MultipleParams::ThreeParamsConst>::is_noexcept);
	#pragma endregion

	#pragma region FunctionTraits::is_noexcept
	static_assert(true  == FunctionTraits(&TwoParamsFree).is_noexcept);
	static_assert(false == FunctionTraits(&ThreeParamsFree).is_noexcept);
	static_assert(true  == FunctionTraits(&MultipleParams::TwoParamsStatic).is_noexcept);
	static_assert(false == FunctionTraits(&MultipleParams::ThreeParamsStatic).is_noexcept);
	static_assert(true  == FunctionTraits(&MultipleParams::TwoParamsNonConst).is_noexcept);
	static_assert(false == FunctionTraits(&MultipleParams::ThreeParamsNonConst).is_noexcept);
	static_assert(true  == FunctionTraits(&MultipleParams::TwoParamsConst).is_noexcept);
	static_assert(false == FunctionTraits(&MultipleParams::ThreeParamsConst).is_noexcept);
	#pragma endregion

	#pragma region FunctionTraits::nth_arg_t
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::nth_arg_t<0>,                       int>);
	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::nth_arg_t<0>,                     int>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::nth_arg_t<0>,     int>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::nth_arg_t<0>,   int>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::nth_arg_t<0>,   int>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::nth_arg_t<0>, int>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::nth_arg_t<0>,      int>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::nth_arg_t<0>,    int>);
	
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::nth_arg_t<1>,                       float>);
	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::nth_arg_t<1>,                     float>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::nth_arg_t<1>,     float>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::nth_arg_t<1>,   float>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::nth_arg_t<1>,   float>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::nth_arg_t<1>, float>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::nth_arg_t<1>,      float>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::nth_arg_t<1>,    float>);

	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::nth_arg_t<2>,                     const std::string&>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::nth_arg_t<2>,   const std::string&>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::nth_arg_t<2>, const std::string&>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::nth_arg_t<2>,    const std::string&>);
	#pragma endregion

	#pragma region Function::nth_arg_t
	static_assert(std::same_as<Function<&TwoParamsFree>::nth_arg_t<0>,                       int>);
	static_assert(std::same_as<Function<&ThreeParamsFree>::nth_arg_t<0>,                     int>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::nth_arg_t<0>,     int>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::nth_arg_t<0>,   int>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::nth_arg_t<0>,   int>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::nth_arg_t<0>, int>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::nth_arg_t<0>,      int>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::nth_arg_t<0>,    int>);

	static_assert(std::same_as<Function<&TwoParamsFree>::nth_arg_t<1>,                       float>);
	static_assert(std::same_as<Function<&ThreeParamsFree>::nth_arg_t<1>,                     float>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::nth_arg_t<1>,     float>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::nth_arg_t<1>,   float>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::nth_arg_t<1>,   float>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::nth_arg_t<1>, float>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::nth_arg_t<1>,      float>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::nth_arg_t<1>,    float>);

	static_assert(std::same_as<Function<&ThreeParamsFree>::nth_arg_t<2>,                     const std::string&>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::nth_arg_t<2>,   const std::string&>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::nth_arg_t<2>, const std::string&>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::nth_arg_t<2>,    const std::string&>);
	#pragma endregion

	#pragma region FunctionTraits::forward
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::forward<std::tuple>,                       std::tuple<std::string, int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::forward<std::tuple>,     std::tuple<std::string, int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::forward<std::tuple>,   std::tuple<std::string, int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::forward<std::tuple>,      std::tuple<std::string, int, float>>);

	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::forward<std::tuple>,                     std::tuple<void, int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::forward<std::tuple>,   std::tuple<void, int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::forward<std::tuple>, std::tuple<void, int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::forward<std::tuple>,    std::tuple<void, int, float, const std::string&>>);

	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::forward<std::tuple, int64_t, uint8_t>,                       std::tuple<int64_t, uint8_t,std::string, int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::forward<std::tuple, int64_t, uint8_t>,     std::tuple<int64_t, uint8_t,std::string, int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::forward<std::tuple, int64_t, uint8_t>,   std::tuple<int64_t, uint8_t,std::string, int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::forward<std::tuple, int64_t, uint8_t>,      std::tuple<int64_t, uint8_t,std::string, int, float>>);

	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::forward<std::tuple, int64_t, uint8_t>,                     std::tuple<int64_t, uint8_t,void, int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::forward<std::tuple, int64_t, uint8_t>,   std::tuple<int64_t, uint8_t,void, int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::forward<std::tuple, int64_t, uint8_t>, std::tuple<int64_t, uint8_t,void, int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::forward<std::tuple, int64_t, uint8_t>,    std::tuple<int64_t, uint8_t,void, int, float, const std::string&>>);
	#pragma endregion

	#pragma region Function::forward
	static_assert(std::same_as<Function<&TwoParamsFree>::forward<std::tuple>,                       std::tuple<std::string, int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::forward<std::tuple>,     std::tuple<std::string, int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::forward<std::tuple>,   std::tuple<std::string, int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::forward<std::tuple>,      std::tuple<std::string, int, float>>);

	static_assert(std::same_as<Function<&ThreeParamsFree>::forward<std::tuple>,                     std::tuple<void, int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::forward<std::tuple>,   std::tuple<void, int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::forward<std::tuple>, std::tuple<void, int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::forward<std::tuple>,    std::tuple<void, int, float, const std::string&>>);

	
	static_assert(std::same_as<Function<&TwoParamsFree>::forward<std::tuple, int64_t, uint8_t>,                       std::tuple<int64_t, uint8_t,std::string, int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::forward<std::tuple, int64_t, uint8_t>,     std::tuple<int64_t, uint8_t,std::string, int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::forward<std::tuple, int64_t, uint8_t>,   std::tuple<int64_t, uint8_t,std::string, int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::forward<std::tuple, int64_t, uint8_t>,      std::tuple<int64_t, uint8_t,std::string, int, float>>);

	static_assert(std::same_as<Function<&ThreeParamsFree>::forward<std::tuple, int64_t, uint8_t>,                     std::tuple<int64_t, uint8_t,void, int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::forward<std::tuple, int64_t, uint8_t>,   std::tuple<int64_t, uint8_t,void, int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::forward<std::tuple, int64_t, uint8_t>, std::tuple<int64_t, uint8_t,void, int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::forward<std::tuple, int64_t, uint8_t>,    std::tuple<int64_t, uint8_t,void, int, float, const std::string&>>);
	#pragma endregion

	#pragma region FunctionTraits::forward_args
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::forward_args<std::tuple>,                       std::tuple<int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::forward_args<std::tuple>,     std::tuple<int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::forward_args<std::tuple>,   std::tuple<int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::forward_args<std::tuple>,      std::tuple<int, float>>);

	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::forward_args<std::tuple>,                     std::tuple<int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::forward_args<std::tuple>,   std::tuple<int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::forward_args<std::tuple>, std::tuple<int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::forward_args<std::tuple>,    std::tuple<int, float, const std::string&>>);

	
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::forward_args<std::tuple, int64_t, uint8_t>,                       std::tuple<int64_t, uint8_t, int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::forward_args<std::tuple, int64_t, uint8_t>,     std::tuple<int64_t, uint8_t, int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::forward_args<std::tuple, int64_t, uint8_t>,   std::tuple<int64_t, uint8_t, int, float>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::forward_args<std::tuple, int64_t, uint8_t>,      std::tuple<int64_t, uint8_t, int, float>>);

	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::forward_args<std::tuple, int64_t, uint8_t>,                     std::tuple<int64_t, uint8_t, int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::forward_args<std::tuple, int64_t, uint8_t>,   std::tuple<int64_t, uint8_t, int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::forward_args<std::tuple, int64_t, uint8_t>, std::tuple<int64_t, uint8_t, int, float, const std::string&>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::forward_args<std::tuple, int64_t, uint8_t>,    std::tuple<int64_t, uint8_t, int, float, const std::string&>>);
	#pragma endregion

	#pragma region Function::forward_args
	static_assert(std::same_as<Function<&TwoParamsFree>::forward_args<std::tuple>,                       std::tuple<int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::forward_args<std::tuple>,     std::tuple<int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::forward_args<std::tuple>,   std::tuple<int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::forward_args<std::tuple>,      std::tuple<int, float>>);

	static_assert(std::same_as<Function<&ThreeParamsFree>::forward_args<std::tuple>,                     std::tuple<int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::forward_args<std::tuple>,   std::tuple<int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::forward_args<std::tuple>, std::tuple<int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::forward_args<std::tuple>,    std::tuple<int, float, const std::string&>>);

	
	static_assert(std::same_as<Function<&TwoParamsFree>::forward_args<std::tuple, int64_t, uint8_t>,                       std::tuple<int64_t, uint8_t, int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::forward_args<std::tuple, int64_t, uint8_t>,     std::tuple<int64_t, uint8_t, int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::forward_args<std::tuple, int64_t, uint8_t>,   std::tuple<int64_t, uint8_t, int, float>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::forward_args<std::tuple, int64_t, uint8_t>,      std::tuple<int64_t, uint8_t, int, float>>);

	static_assert(std::same_as<Function<&ThreeParamsFree>::forward_args<std::tuple, int64_t, uint8_t>,                     std::tuple<int64_t, uint8_t, int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::forward_args<std::tuple, int64_t, uint8_t>,   std::tuple<int64_t, uint8_t, int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::forward_args<std::tuple, int64_t, uint8_t>, std::tuple<int64_t, uint8_t, int, float, const std::string&>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::forward_args<std::tuple, int64_t, uint8_t>,    std::tuple<int64_t, uint8_t, int, float, const std::string&>>);
	#pragma endregion

	#pragma region FunctionTraits::apply_signature
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::apply_signature<std::function>,                       std::function<std::string(int, float)>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::apply_signature<std::function>,     std::function<std::string(int, float)>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::apply_signature<std::function>,   std::function<std::string(int, float)>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::apply_signature<std::function>,      std::function<std::string(int, float)>>);

	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::apply_signature<std::function>,                     std::function<void(int, float, const std::string&)>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::apply_signature<std::function>,   std::function<void(int, float, const std::string&)>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::apply_signature<std::function>, std::function<void(int, float, const std::string&)>>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::apply_signature<std::function>,    std::function<void(int, float, const std::string&)>>);
	#pragma endregion

	#pragma region Function::apply_signature
	static_assert(std::same_as<Function<&TwoParamsFree>::apply_signature<std::function>,                       std::function<std::string(int, float)>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::apply_signature<std::function>,     std::function<std::string(int, float)>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::apply_signature<std::function>,   std::function<std::string(int, float)>>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::apply_signature<std::function>,      std::function<std::string(int, float)>>);

	static_assert(std::same_as<Function<&ThreeParamsFree>::apply_signature<std::function>,                     std::function<void(int, float, const std::string&)>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::apply_signature<std::function>,   std::function<void(int, float, const std::string&)>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::apply_signature<std::function>, std::function<void(int, float, const std::string&)>>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::apply_signature<std::function>,    std::function<void(int, float, const std::string&)>>);
	#pragma endregion

	#pragma region FunctionTraits::class_type
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::class_type,                       void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::class_type,     void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::class_type,   MultipleParams>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::class_type,      MultipleParams>);

	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::class_type,                     void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::class_type,   void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::class_type, MultipleParams>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::class_type,    MultipleParams>);
	#pragma endregion

	#pragma region Function::class_type
	static_assert(std::same_as<Function<&TwoParamsFree>::class_type,                       void>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::class_type,     void>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::class_type,   MultipleParams>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::class_type,      MultipleParams>);

	static_assert(std::same_as<Function<&ThreeParamsFree>::class_type,                     void>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::class_type,   void>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::class_type, MultipleParams>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::class_type,    MultipleParams>);
	#pragma endregion

	#pragma region FunctionTraits::target_type
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::target_type,                       void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::target_type,     void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::target_type,   MultipleParams*>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::target_type,      const MultipleParams*>);

	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::target_type,                     void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::target_type,   void>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::target_type, MultipleParams*>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::target_type,    const MultipleParams*>);
	#pragma endregion

	#pragma region Function::target_type
	static_assert(std::same_as<Function<&TwoParamsFree>::target_type,                       void>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::target_type,     void>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::target_type,   MultipleParams*>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::target_type,      const MultipleParams*>);

	static_assert(std::same_as<Function<&ThreeParamsFree>::target_type,                     void>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::target_type,   void>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::target_type, MultipleParams*>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::target_type,    const MultipleParams*>);
	#pragma endregion

	#pragma region FunctionTraits::raw_ptr_t
	static_assert(std::same_as<decltype(FunctionTraits(&TwoParamsFree))::raw_ptr_t,                       std::string(*)(int, float) noexcept>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsStatic))::raw_ptr_t,     std::string(*)(int, float) noexcept>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsNonConst))::raw_ptr_t,   std::string(MultipleParams::*)(int, float) noexcept>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::TwoParamsConst))::raw_ptr_t,      std::string(MultipleParams::*)(int, float) const noexcept>);

	static_assert(std::same_as<decltype(FunctionTraits(&ThreeParamsFree))::raw_ptr_t,                     void(*)(int, float, const std::string&)>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsStatic))::raw_ptr_t,   void(*)(int, float, const std::string&)>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsNonConst))::raw_ptr_t, void(MultipleParams::*)(int, float, const std::string&)>);
	static_assert(std::same_as<decltype(FunctionTraits(&MultipleParams::ThreeParamsConst))::raw_ptr_t,    void(MultipleParams::*)(int, float, const std::string&) const>);
	#pragma endregion

	#pragma region Function::raw_ptr_t
	static_assert(std::same_as<Function<&TwoParamsFree>::raw_ptr_t,                       std::string(*)(int, float) noexcept>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsStatic>::raw_ptr_t,     std::string(*)(int, float) noexcept>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsNonConst>::raw_ptr_t,   std::string(MultipleParams::*)(int, float) noexcept>);
	static_assert(std::same_as<Function<&MultipleParams::TwoParamsConst>::raw_ptr_t,      std::string(MultipleParams::*)(int, float) const noexcept>);

	static_assert(std::same_as<Function<&ThreeParamsFree>::raw_ptr_t,                     void(*)(int, float, const std::string&)>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsStatic>::raw_ptr_t,   void(*)(int, float, const std::string&)>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsNonConst>::raw_ptr_t, void(MultipleParams::*)(int, float, const std::string&)>);
	static_assert(std::same_as<Function<&MultipleParams::ThreeParamsConst>::raw_ptr_t,    void(MultipleParams::*)(int, float, const std::string&) const>);
	#pragma endregion
}