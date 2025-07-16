#include <StdExt/FunctionPtr.h>
#include <StdExt/Test/Test.h>
#include <StdExt/Platform.h>

#include <array>
#include <functional>
#include <string>
#include <vector>

#ifdef STD_EXT_WIN32
#	pragma warning(push)
#	pragma warning( disable : 4244 )
#endif

enum class FuncType
{
	FreeStaticNoExcept,
	FreeStaticExcept,
	MemberStaticNoExcept,
	MemberStaticExcept,
	ConstMemberExcept,
	ConstMemberNoExcept,
	NonConstMemberExcept,
	NonConstMemberNoExcept
};

static std::vector<FuncType> FuncTypeCallOrder;

static int FreeExcept(float i)
{
	FuncTypeCallOrder.emplace_back(FuncType::FreeStaticExcept);

	if (i < 0)
		throw std::invalid_argument("i < 0");
	
	return static_cast<int>(i) + 2;
}

static float FreeNoExcept(int i) noexcept
{
	FuncTypeCallOrder.emplace_back(FuncType::FreeStaticNoExcept);

	return static_cast<float>(i + 2);
}

struct StaticExceptTest
{
	static int Except(float f)
	{
		FuncTypeCallOrder.emplace_back(FuncType::MemberStaticExcept);

		if (f < 0.0f)
			throw std::invalid_argument("i < 0");
		
		return static_cast<int>(f) + 2;
	}

	static float NoExcept(int i) noexcept
	{
		FuncTypeCallOrder.emplace_back(FuncType::MemberStaticNoExcept);

		return static_cast<float>(i) + 2.0f;
	}
};

class NonStatic
{
private:
	float mValue;

public:
	constexpr NonStatic()
		: mValue(0.0f)
	{
	}

	int Except(float i)
	{
		FuncTypeCallOrder.emplace_back(FuncType::NonConstMemberExcept);

		if (i < 0.0f)
			throw std::invalid_argument("i < 0");
		
		mValue = static_cast<float>(i) + 2.0f;
		return static_cast<int>(mValue);
	}

	float NoExcept(int i) noexcept
	{
		FuncTypeCallOrder.emplace_back(FuncType::NonConstMemberNoExcept);

		mValue = static_cast<float>(i) + 2.0f;
		return mValue;
	}

	int ConstExcept(float i) const
	{
		FuncTypeCallOrder.emplace_back(FuncType::ConstMemberExcept);

		if (i < 0.0f)
			throw std::invalid_argument("i < 0");
		
		return static_cast<int>(i) + 2;
	}

	float ConstNoExcept(int i) const noexcept
	{
		FuncTypeCallOrder.emplace_back(FuncType::ConstMemberNoExcept);

		return static_cast<float>(i + 2);
	}
};

class BaseClass
{
protected:
	int mValue;

public:
	constexpr BaseClass()
	{
		mValue = 0;
	}

	int nonVirtualFunc(int i_val, float f_val)
	{
		mValue = 1 + i_val + static_cast<int>(f_val);
		return mValue;
	}

	virtual int virtualFunc(int i_val, float f_val)
	{
		mValue = 1 + i_val + static_cast<int>(f_val);
		return mValue;
	}

	virtual int pureVirtualFunc(int i_val, float f_val) = 0;

	int getValue() const noexcept
	{
		return mValue;
	}
};

class DerivedClass : public BaseClass
{
public:
	constexpr DerivedClass()
		: BaseClass()
	{
	}

	int nonVirtualFunc(int i_val, float f_val)
	{
		mValue = 2 + i_val + static_cast<int>(f_val);
		return mValue;
	}

	int virtualFunc(int i_val, float f_val) override
	{
		mValue = 2 + i_val + static_cast<int>(f_val);
		return mValue;
	}

	int pureVirtualFunc(int i_val, float f_val) override
	{
		mValue = 3 + i_val + static_cast<int>(f_val);
		return mValue;
	}
};

class TestClass
{
private:
	int mValue{};

public:
	constexpr TestClass()
		: mValue(0)
	{
	}

	constexpr int addValue(int i) const
	{
		return mValue + i;
	}

	void setValue(int value)
	{
		mValue = value;
	}

	static int makeValue(int i, int j)
	{
		return i + j;
	}

	static int StaticNoExcept(int i) noexcept
	{
		return i + 2;
	}

	static int StaticExcept(int i)
	{
		if (i < 0)
			throw std::invalid_argument("Argument must be positive.");

		return i + 2;
	}

	static TestClass makeTestClass(int value, std::string name)
	{
		TestClass ret;
		ret.mValue = value;

		return ret;
	}

	static int ambiguous(float f_param, int i_param)
	{
		return 5 + static_cast<int>(f_param) + i_param;
	}

	float ambiguous(int i_param, float f_param) const
	{
		return static_cast<float>(2 + mValue + i_param + static_cast<int>(f_param));
	}

	int ambiguous(int i_param, float f_param)
	{
		return 2 + mValue + i_param + static_cast<int>(f_param);
	}
};

using namespace StdExt;
using namespace StdExt::Test;

template<CallableWith<void, int> auto... calls>
class TemplatedCall
{
public:
	static void callAll(int arg)
	{
		(
			calls(arg),
			...
		);
	}
};

template<CallableWith<void, int> auto... calls>
class TemplatedCallPtr
{
public:
	static void callAll(int arg)
	{
		(
			CallablePtr<void(int)>(&calls)(arg),
			...
		);
	}
};

template<CallableWith<float, int> auto... calls>
class TemplatedReturnCallPtr
{
public:
	static void callAll(int arg)
	{
		(
			CallablePtr<float(int)>(&calls)(arg),
			...
		);
	}
};

void testFunctionPtr()
{
	DerivedClass derived_object;
	BaseClass* base_pointer = &derived_object;

	{
		const auto base_virtual_call = bind<&BaseClass::virtualFunc>(base_pointer);
		const auto base_pure_virtual_call = bind<&BaseClass::pureVirtualFunc>(base_pointer);
		const auto derived_call_to_base_func = bind<&DerivedClass::getValue>(&derived_object);
		const auto base_non_virtual_call = bind<&BaseClass::nonVirtualFunc>(&derived_object);
		const auto derived_non_virtual_call = bind<&DerivedClass::nonVirtualFunc>(&derived_object);

		Test::testForResult(
			"Templated Bind - Bind by a base function pointer to derived object calls derived override.",
			5, base_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Templated Bind - Bind by a base pure virtual function pointer to derived object calls derived override.",
			6, base_pure_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Templated Bind - Non-virtual call invokes base according to the function pointer.",
			4, base_non_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Templated Bind - Non-virtual call invokes derived according to the function pointer.",
			5, derived_non_virtual_call(1.0f, 2.0f)
		);
	}

	{
		const auto base_virtual_call = bind(&BaseClass::virtualFunc, base_pointer);
		const auto base_pure_virtual_call = bind(&BaseClass::pureVirtualFunc, base_pointer);
		const auto base_non_virtual_call = bind(&BaseClass::nonVirtualFunc, &derived_object);
		const auto derived_non_virtual_call = bind(&DerivedClass::nonVirtualFunc, &derived_object);

		Test::testForResult(
			"Function Pointer Bind - Bind by a base function pointer to derived object calls derived override.",
			5, base_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Function Pointer Bind - Bind by a base pure virtual function pointer to derived object calls derived override.",
			6, base_pure_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Function Pointer Bind - Non-virtual call invokes base according to the function pointer.",
			4, base_non_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Function Pointer Bind - Non-virtual call invokes derived according to the function pointer.",
			5, derived_non_virtual_call(1.0f, 2.0f)
		);
	}

	{
		static constinit NonStatic non_static_obj;

		constexpr std::array expected{
			FuncType::FreeStaticExcept, FuncType::FreeStaticNoExcept,
			FuncType::MemberStaticExcept, FuncType::MemberStaticNoExcept,
			FuncType::ConstMemberExcept, FuncType::ConstMemberNoExcept,
			FuncType::NonConstMemberExcept, FuncType::NonConstMemberNoExcept
		};

		auto test_call_chain = [&]()
		{
			if (expected.size() != FuncTypeCallOrder.size())
			{
				return false;
			}

			for(size_t i = 0; i < expected.size(); ++i)
			{
				if ( expected[i] != FuncTypeCallOrder[i])
					return false;
			}

			return true;
		};

		TemplatedCall<
			bind<&FreeExcept>(),
			bind<&FreeNoExcept>(),
			bind<&StaticExceptTest::Except>(),
			bind<&StaticExceptTest::NoExcept>(),
			bind<&NonStatic::ConstExcept>(&non_static_obj),
			bind<&NonStatic::ConstNoExcept>(&non_static_obj),
			bind<&NonStatic::Except>(&non_static_obj),
			bind<&NonStatic::NoExcept>(&non_static_obj)
		> templated_call;

		FuncTypeCallOrder.clear();
		templated_call.callAll(1);
		Test::testByCheck("Templated function bindings are constexpr compatible and call expected functions.", test_call_chain);

		TemplatedCall<
			bind(&FreeExcept),
			bind(&FreeNoExcept),
			bind(&StaticExceptTest::Except),
			bind(&StaticExceptTest::NoExcept),
			bind(&NonStatic::ConstExcept, &non_static_obj),
			bind(&NonStatic::ConstNoExcept, &non_static_obj),
			bind(&NonStatic::Except ,&non_static_obj),
			bind(&NonStatic::NoExcept, &non_static_obj)
		> pointer_call;

		FuncTypeCallOrder.clear();
		pointer_call.callAll(1);
		Test::testByCheck("Pointer function bindings are constexpr compatible and call expected functions.", test_call_chain);

		TemplatedCallPtr<
			bind<&FreeExcept>(),
			bind<&FreeNoExcept>(),
			bind<&StaticExceptTest::Except>(),
			bind<&StaticExceptTest::NoExcept>(),
			bind<&NonStatic::ConstExcept>(&non_static_obj),
			bind<&NonStatic::ConstNoExcept>(&non_static_obj),
			bind<&NonStatic::Except>(&non_static_obj),
			bind<&NonStatic::NoExcept>(&non_static_obj)
		> call_templated_call;

		FuncTypeCallOrder.clear();
		call_templated_call.callAll(1);
		Test::testByCheck("CallablePtr calls expected function for each callable type constructed by template based bind().", test_call_chain);

		TemplatedCallPtr<
			bind(&FreeExcept),
			bind(&FreeNoExcept),
			bind(&StaticExceptTest::Except),
			bind(&StaticExceptTest::NoExcept),
			bind(&NonStatic::ConstExcept, &non_static_obj),
			bind(&NonStatic::ConstNoExcept, &non_static_obj),
			bind(&NonStatic::Except ,&non_static_obj),
			bind(&NonStatic::NoExcept, &non_static_obj)
		> call_pointer_call;

		FuncTypeCallOrder.clear();
		call_pointer_call.callAll(1);
		Test::testByCheck("CallablePtr calls expected function for each callable type constructed by pointer based bind().", test_call_chain);

		TemplatedReturnCallPtr<
			bind(&FreeExcept),
			bind(&FreeNoExcept),
			bind(&StaticExceptTest::Except),
			bind(&StaticExceptTest::NoExcept),
			bind(&NonStatic::ConstExcept, &non_static_obj),
			bind(&NonStatic::ConstNoExcept, &non_static_obj),
			bind(&NonStatic::Except ,&non_static_obj),
			bind(&NonStatic::NoExcept, &non_static_obj)
		> call_return_pointer_call;

		FuncTypeCallOrder.clear();
		call_return_pointer_call.callAll(1);
		Test::testByCheck("CallablePtr with return calls expected function for each callable type constructed by pointer based bind().", test_call_chain);

		{
			std::tuple non_const_binds {
				bind(&FreeExcept),
				bind(&FreeNoExcept),
				bind(&StaticExceptTest::Except),
				bind(&StaticExceptTest::NoExcept),
				bind(&NonStatic::ConstExcept, &non_static_obj),
				bind(&NonStatic::ConstNoExcept, &non_static_obj),
				bind(&NonStatic::Except, &non_static_obj),
				bind(&NonStatic::NoExcept, &non_static_obj)
			};

			std::array<CallablePtr<void(int)>, 8> non_const_void_call_ptrs {
				CallablePtr<void(int)>(&std::get<0>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<1>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<2>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<3>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<4>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<5>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<6>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<7>(non_const_binds))
			};

			FuncTypeCallOrder.clear();
			for (auto& call_ptr : non_const_void_call_ptrs)
			{
				call_ptr(1);
			}
			Test::testByCheck("Non-constant CallablePtr calls expected function for each callable type constructed by pointer based bind().", test_call_chain);

			std::array<CallablePtr<float(int)>, 8> non_const_return_call_ptrs {
				CallablePtr<float(int)>(&std::get<0>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<1>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<2>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<3>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<4>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<5>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<6>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<7>(non_const_binds))
			};

			FuncTypeCallOrder.clear();
			for (auto& call_ptr : non_const_return_call_ptrs)
			{
				call_ptr(1);
			}
			Test::testByCheck("Non-constant CallablePtr with return calls expected function for each callable type constructed by pointer based bind().", test_call_chain);
		}

		{
			std::tuple non_const_binds {
				bind<&FreeExcept>(),
				bind<&FreeNoExcept>(),
				bind<&StaticExceptTest::Except>(),
				bind<&StaticExceptTest::NoExcept>(),
				bind<&NonStatic::ConstExcept>(&non_static_obj),
				bind<&NonStatic::ConstNoExcept>(&non_static_obj),
				bind<&NonStatic::Except>(&non_static_obj),
				bind<&NonStatic::NoExcept>(&non_static_obj)
			};

			std::array<CallablePtr<void(int)>, 8> non_const_void_call_ptrs {
				CallablePtr<void(int)>(&std::get<0>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<1>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<2>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<3>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<4>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<5>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<6>(non_const_binds)),
				CallablePtr<void(int)>(&std::get<7>(non_const_binds))
			};

			FuncTypeCallOrder.clear();
			for (auto& call_ptr : non_const_void_call_ptrs)
			{
				call_ptr(1);
			}
			Test::testByCheck("Non-constant CallablePtr calls expected function for each callable type constructed by template based bind().", test_call_chain);

			std::array<CallablePtr<float(int)>, 8> non_const_return_call_ptrs {
				CallablePtr<float(int)>(&std::get<0>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<1>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<2>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<3>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<4>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<5>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<6>(non_const_binds)),
				CallablePtr<float(int)>(&std::get<7>(non_const_binds))
			};

			FuncTypeCallOrder.clear();
			for (auto& call_ptr : non_const_return_call_ptrs)
			{
				call_ptr(1);
			}
			Test::testByCheck("Non-constant CallablePtr with return calls expected function for each callable type constructed by template based bind().", test_call_chain);
		}
	}
}

#ifdef STD_EXT_WIN32
#	pragma warning(pop)
#endif