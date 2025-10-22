#include <StdExt/Concepts.h>

#include <StdExt/Callable.h>
#include <StdExt/Utility.h>
#include <StdExt/Type.h>

#include "TestClasses.h"

#include <functional>
#include <string>
#include <tuple>
#include <vector>

using namespace StdExt;

class BaseClass
{
private:
	int mBaseVal;

public:
	BaseClass()
		: mBaseVal(0)
	{
	}

	BaseClass(int val)
		: mBaseVal(val)
	{
	}

	virtual ~BaseClass()
	{
	}
};

class SubClass : public BaseClass
{
private:
	int mSubVal;

public:
	SubClass()
		: BaseClass(), mSubVal(0)
	{
	}

	SubClass(int base_val, int sub_val)
		: BaseClass(base_val), mSubVal(0)
	{
	}
};

class SubClassMoveOnly : public BaseClass
{
public:
	SubClassMoveOnly(SubClassMoveOnly&&) = default;
	SubClassMoveOnly(const SubClassMoveOnly&) = delete;

	SubClassMoveOnly() = default;
};

class TrivialCopyable
{
private:
	int mBaseVal;

public:
	TrivialCopyable(const TrivialCopyable&) = default;
	TrivialCopyable& operator=(const TrivialCopyable&) = default;

	TrivialCopyable(int val)
	{
		mBaseVal = val;
	}

	TrivialCopyable(TrivialCopyable&& other)
	{
		mBaseVal = other.mBaseVal;
		other.mBaseVal = 0;
	}
};

class TrivialMovable
{
private:
	int mBaseVal;

public:
	TrivialMovable(TrivialMovable&& other) = default;

	TrivialMovable(int val)
	{
		mBaseVal = val;
	}

	TrivialMovable(const TrivialMovable& other)
	{
		mBaseVal = other.mBaseVal;
	}
};

class PrefixIncrement
{
private:
	int mValue;

public:
	PrefixIncrement(int val = 0)
		: mValue(val)
	{
	}

	PrefixIncrement& operator++()
	{
		++mValue;
		return *this;
	}
};

class PostfixIncrement
{
private:
	int mValue;

public:
	PostfixIncrement(int val = 0)
		: mValue(val)
	{
	}

	PostfixIncrement operator++(int)
	{
		PostfixIncrement ret(mValue);
		++mValue;
		return ret;
	}
};

class PrefixDecrement
{
private:
	int mValue;

public:
	PrefixDecrement(int val = 0)
		: mValue(val)
	{
	}

	PrefixDecrement& operator--()
	{
		--mValue;
		return *this;
	}
};

class PostfixDecrement
{
private:
	int mValue;

public:
	PostfixDecrement(int val = 0)
		: mValue(val)
	{
	}

	PostfixDecrement operator--(int)
	{
		PostfixDecrement ret(mValue);
		--mValue;
		return ret;
	}
};

template<SubclassOf<Dog> T>
void foo(T dog)
{
}

void concept_test()
{
	foo(Dog());
	foo(Pug());
	
#pragma region Mem Movable/Copyable
	static_assert(  MemMovable<TrivialMovable> );
	static_assert( !MemMovable<TrivialCopyable> );
	static_assert( !MemCopyable<TrivialMovable> );
	static_assert(  MemCopyable<TrivialCopyable> );
	static_assert(  MemMovable<int> );
	static_assert( !MemCopyable<SubClass> );
	static_assert( !MemMovable<SubClass> );
	static_assert( !MemCopyable<std::string> );
	static_assert( !MemMovable<std::string> );
	static_assert(  MemCopyable<int> );
	static_assert(  MemMovable<int*> );
	static_assert(  MemCopyable<int*> );
#pragma endregion

#pragma region AnyOf
	static_assert(  AnyOf<int, float, int, std::string> );
	static_assert( !AnyOf<float, uint32_t, double, std::string> );
	static_assert( !AnyOf<double, uint32_t, int64_t, std::string, float> );
	static_assert(  AnyOf<SubClass, float, int, std::string, SubClass, std::vector<int>> );
	static_assert( !AnyOf<std::vector<int>, float, int, std::string, std::vector<float>> );
	static_assert( !AnyOf<BaseClass, float, int, SubClass, std::vector<float>> );
	static_assert( !AnyOf<SubClass, float, int, std::string, BaseClass> );
#pragma endregion

#pragma region Type<int>::stripped_t
	static_assert( std::is_same_v<Type<int>::stripped_t, int> );
	static_assert( std::is_same_v<Type<int*>::stripped_t, int> );
	static_assert( std::is_same_v<Type<const int>::stripped_t, int> );

	static_assert( std::is_same_v<Type<std::string>::stripped_t, std::string> );
	static_assert( std::is_same_v<Type<std::string*>::stripped_t, std::string> );
	static_assert( std::is_same_v<Type<const std::string*>::stripped_t, std::string> );

	static_assert( std::is_same_v<Type<std::string>::stripped_t, std::string> );
	static_assert( std::is_same_v<Type<std::string&>::stripped_t, std::string> );
	static_assert( std::is_same_v<Type<const std::string&>::stripped_t, std::string> );
#pragma endregion

#pragma region Type::stripped_ptr_ref_t
	static_assert( std::is_same_v<Type<int>::stripped_ptr_ref_t, int> );
	static_assert( std::is_same_v<Type<int*>::stripped_ptr_ref_t, int> );
	static_assert( std::is_same_v<Type<const int>::stripped_ptr_ref_t, const int> );

	static_assert( std::is_same_v<Type<std::string>::stripped_ptr_ref_t, std::string> );
	static_assert( std::is_same_v<Type<std::string*>::stripped_ptr_ref_t, std::string> );
	static_assert( std::is_same_v<Type<const std::string*>::stripped_ptr_ref_t, const std::string> );

	static_assert( std::is_same_v<Type<std::string&>::stripped_ptr_ref_t, std::string> );
	static_assert( std::is_same_v<Type<const std::string&>::stripped_ptr_ref_t, const std::string> );
#pragma endregion

#pragma region Type::arg_non_copy_t
	static_assert( std::is_same_v<Type<int>::arg_non_copy_t, int> );
	static_assert( std::is_same_v<Type<int*>::arg_non_copy_t, int*> );
	static_assert( std::is_same_v<Type<const int>::arg_non_copy_t, const int> );

	static_assert( std::is_same_v<Type<std::string>::arg_non_copy_t, const std::string&> );
	static_assert( std::is_same_v<Type<std::string*>::arg_non_copy_t, std::string*> );
	static_assert( std::is_same_v<Type<const std::string*>::arg_non_copy_t, const std::string*> );

	static_assert( std::is_same_v<Type<std::string&>::arg_non_copy_t, std::string&> );
	static_assert( std::is_same_v<Type<std::string&&>::arg_non_copy_t, std::string&&> );
	static_assert( std::is_same_v<Type<const std::string&>::arg_non_copy_t, const std::string&> );
#pragma endregion

#pragma region Type::core
	static_assert( std::same_as<Type<const std::string&>::core, std::string> );
	static_assert( std::same_as<Type<std::string&>::core,       std::string> );
	static_assert( std::same_as<Type<const std::string*>::core, std::string> );
	static_assert( std::same_as<Type<std::string*>::core,       std::string> );
	static_assert( std::same_as<Type<std::string>::core,        std::string> );
#pragma endregion

#pragma region PointerType
	static_assert( !PointerType<int> );
	static_assert(  PointerType<int*> );
	static_assert( !PointerType<const int> );

	static_assert( !PointerType<std::string> );
	static_assert(  PointerType<std::string*> );
	static_assert(  PointerType<std::string* const> );
	static_assert(  PointerType<const std::string*> );
	static_assert( !PointerType<const std::string&> );
	static_assert( !PointerType<const std::string> );
#pragma endregion

#pragma region ReferenceType
	static_assert( !ReferenceType<int> );
	static_assert( !ReferenceType<int*> );
	static_assert( !ReferenceType<const int> );

	static_assert( !ReferenceType<std::string> );
	static_assert( !ReferenceType<std::string*> );
	static_assert( !ReferenceType<std::string* const> );
	static_assert( !ReferenceType<const std::string*> );
	static_assert(  ReferenceType<const std::string&> );
	static_assert( !ReferenceType<const std::string> );

	static_assert( !ReferenceType<const std::string*, const std::string&, const std::string> );
	static_assert(  ReferenceType<const std::string&, const int&> );
#pragma endregion

#pragma region ConstType
	static_assert( !ConstType<int> );
	static_assert( !ConstType<int*> );
	static_assert(  ConstType<const int> );

	static_assert( !ConstType<std::string> );
	static_assert( !ConstType<std::string*> );
	static_assert( !ConstType<std::string* const> );
	static_assert(  ConstType<const std::string*> );
	static_assert(  ConstType<const std::string&> );
	static_assert(  ConstType<const std::string> );
#pragma endregion

#pragma region NonConstType
	static_assert(  NonConstType<int> );
	static_assert(  NonConstType<int*> );
	static_assert( !NonConstType<const int> );

	static_assert(  NonConstType<std::string> );
	static_assert(  NonConstType<std::string*> );
	static_assert(  NonConstType<std::string* const> );
	static_assert( !NonConstType<const std::string*> );
	static_assert( !NonConstType<const std::string&> );
	static_assert( !NonConstType<const std::string> );
#pragma endregion

#pragma region ConstReferenceType
	static_assert( !ConstReferenceType<int> );
	static_assert( !ConstReferenceType<int*> );
	static_assert( !ConstReferenceType<const int> );

	static_assert( !ConstReferenceType<std::string> );
	static_assert( !ConstReferenceType<std::string&> );
	static_assert( !ConstReferenceType<std::string*> );
	static_assert( !ConstReferenceType<std::string* const> );
	static_assert( !ConstReferenceType<const std::string*> );
	static_assert(  ConstReferenceType<const std::string&> );
	static_assert( !ConstReferenceType<const std::string> );
#pragma endregion

#pragma region AssignableFrom and AssignableTo
	static_assert(  AssignableFrom<const BaseClass*, SubClass*> );
	static_assert( !AssignableFrom<SubClass*, BaseClass*> );
	static_assert(  AssignableFrom<BaseClass&, SubClass> );
	static_assert( !AssignableFrom<SubClass&, BaseClass> );
	static_assert( !AssignableFrom<SubClassMoveOnly, SubClassMoveOnly> );
	static_assert(  AssignableFrom<BaseClass*, SubClassMoveOnly*> );
	static_assert(  AssignableFrom<BaseClass, const BaseClass&> );

	static_assert(  AssignableTo<SubClass*, const BaseClass*> );
	static_assert( !AssignableTo<BaseClass*, SubClass*> );
	static_assert(  AssignableTo<SubClass, BaseClass&> );
	static_assert( !AssignableTo<BaseClass, SubClass&> );
	static_assert( !AssignableTo<SubClassMoveOnly, SubClassMoveOnly> );
	static_assert(  AssignableTo<SubClassMoveOnly*, BaseClass*> );
	static_assert(  AssignableTo<const BaseClass&, BaseClass> );
#pragma endregion

#pragma region ImplicitlyConvertableTo
	static_assert(  ImplicitlyConvertableTo<const BaseClass*, SubClass*> );
	static_assert(  ImplicitlyConvertableTo<const BaseClass&, SubClass&&> );
	static_assert( !ImplicitlyConvertableTo<BaseClass&&, const BaseClass&> );
	static_assert( !ImplicitlyConvertableTo<SubClass*, BaseClass*> );
	static_assert(  ImplicitlyConvertableTo<BaseClass&, SubClass> );
	static_assert( !ImplicitlyConvertableTo<SubClass&, BaseClass> );
	static_assert( !ImplicitlyConvertableTo<SubClassMoveOnly, SubClassMoveOnly> );
	static_assert(  ImplicitlyConvertableTo<BaseClass*, SubClassMoveOnly*> );
	static_assert(  ImplicitlyConvertableTo<float, int> );

	static_assert(  ImplicitlyConvertableTo<const BaseClass*, SubClassMoveOnly*, SubClass*> );
	static_assert(  ImplicitlyConvertableTo<const BaseClass&, SubClassMoveOnly, SubClass&&> );
#pragma endregion

#pragma region Class Hierachy
	static_assert(  SubclassOf<Dog, Animal> );
	static_assert(  SubclassOf<Dog, Dog> );
	static_assert(  SuperclassOf<Animal, Dog> );
	static_assert(  SuperclassOf<Dog, Dog> );
	static_assert(  InHeirarchyOf<Animal, Dog> );
	static_assert(  InHeirarchyOf<Dog, Animal> );
	static_assert(  InHeirarchyOf<Dog, Dog> );
#pragma endregion


#pragma region Template Specialization
	static_assert( !SpecializationOf<int, std::vector> );
	static_assert(  SpecializationOf<std::vector<int>, std::vector> );
	static_assert( !SpecializationOf<std::vector<int>, std::tuple> );
	static_assert(  SpecializationOf<std::function<int(float)>, std::function> );
	static_assert( !SpecializationOf<std::function<int(float)>, std::tuple> );
#pragma endregion

#pragma region Increment and Decrement
	static_assert(  HasPrefixIncrement<PrefixIncrement> );
	static_assert( !HasPrefixIncrement<PrefixDecrement> );
	static_assert( !HasPrefixIncrement<PostfixIncrement> );
	static_assert( !HasPrefixIncrement<PostfixDecrement> );

	static_assert( !HasPostfixIncrement<PrefixIncrement> );
	static_assert( !HasPostfixIncrement<PrefixDecrement> );
	static_assert(  HasPostfixIncrement<PostfixIncrement> );
	static_assert( !HasPostfixIncrement<PostfixDecrement> );

	static_assert( !HasPrefixDecrement<PrefixIncrement> );
	static_assert(  HasPrefixDecrement<PrefixDecrement> );
	static_assert( !HasPrefixDecrement<PostfixIncrement> );
	static_assert( !HasPrefixDecrement<PostfixDecrement> );

	static_assert( !HasPostfixDecrement<PrefixIncrement> );
	static_assert( !HasPostfixDecrement<PrefixDecrement> );
	static_assert( !HasPostfixDecrement<PostfixIncrement> );
	static_assert(  HasPostfixDecrement<PostfixDecrement> );
	
	static_assert( !HasPrefixIncrement<const PrefixIncrement> );
	static_assert( !HasPrefixDecrement<const PrefixDecrement> );
	static_assert( !HasPostfixDecrement<const PostfixIncrement> );
	static_assert( !HasPostfixDecrement<const PostfixDecrement> );

	static_assert( !HasPrefixIncrement<bool> );
	static_assert( !HasPrefixDecrement<bool> );
	static_assert( !HasPostfixDecrement<bool> );
	static_assert( !HasPostfixDecrement<bool> );

	static_assert(  HasPrefixIncrement<float> );
	static_assert(  HasPrefixDecrement<float> );
	static_assert(  HasPostfixIncrement<float> );
	static_assert(  HasPostfixDecrement<float> );

	static_assert(  HasPrefixIncrement<BaseClass*> );
	static_assert(  HasPrefixDecrement<BaseClass*> );
	static_assert(  HasPostfixIncrement<BaseClass*> );
	static_assert(  HasPostfixDecrement<BaseClass*> );

	static_assert(  HasPrefixIncrement<const BaseClass*> );
	static_assert(  HasPrefixDecrement<const BaseClass*> );
	static_assert(  HasPostfixIncrement<const BaseClass*> );
	static_assert(  HasPostfixDecrement<const BaseClass*> );

	static_assert( !HasPrefixIncrement<BaseClass* const> );
	static_assert( !HasPrefixDecrement<BaseClass* const> );
	static_assert( !HasPostfixIncrement<BaseClass* const> );
	static_assert( !HasPostfixDecrement<BaseClass* const> );
#pragma endregion

#pragma region CallableWith
	{
		auto function = [](int i, int j = 0) {};
		auto returns_void = [](int i) {};
		auto returns_string = [](int i) { return std::to_string(i); };
		auto returns_float = [](int i) { return static_cast<float>(i); };
		
		static_assert( !CallableWith<decltype(returns_void), AnyReturn, int> );
		static_assert(  CallableWith<decltype(returns_string), AnyReturn, int> );
		static_assert(  CallableWith<decltype(returns_float), AnyReturn, int> );

		static_assert(  CallableWith<decltype(function), void, int> );
		static_assert(  CallableWith<decltype(function), void, int, int> );
		static_assert(  CallableWith<decltype(function), void, float, int> );
		static_assert(  CallableWith<decltype(function), void, float> );
		static_assert( !CallableWith<decltype(function), int, int> );
		static_assert( !CallableWith<decltype(function), void, std::string> );
	}
	{
		auto function = [](const std::string& str) -> std::string
		{
			return str;
		};

		static_assert(  CallableWith<decltype(function), void, std::string> );
		static_assert(  CallableWith<decltype(function), std::string, std::string> );
		static_assert( !CallableWith<decltype(function), int, std::string> );
		static_assert( !CallableWith<decltype(function), SubClassMoveOnly> );
		static_assert( !CallableWith<decltype(function), SubClassMoveOnly, int> );
		static_assert( !CallableWith<decltype(function), SubClassMoveOnly, std::string> );
		static_assert( !CallableWith<decltype(function), int, std::string> );

		static_assert(
			CallableWith<decltype(
				[](int i)
				{

				}
			), void, int>
		);

		static_assert( !Integral<float> );
		static_assert( !Integral<double> );
		static_assert(  Integral<uint8_t> );
		static_assert(  Integral<uint16_t> );
		static_assert(  Integral<uint32_t> );
		static_assert(  Integral<uint64_t> );
		static_assert(  Integral<int8_t> );
		static_assert(  Integral<int16_t> );
		static_assert(  Integral<int32_t> );
		static_assert(  Integral<int64_t> );

		static_assert(  FloatingPoint<float> );
		static_assert(  FloatingPoint<double> );
		static_assert( !FloatingPoint<uint8_t> );
		static_assert( !FloatingPoint<uint16_t> );
		static_assert( !FloatingPoint<uint32_t> );
		static_assert( !FloatingPoint<uint64_t> );
		static_assert( !FloatingPoint<int8_t> );
		static_assert( !FloatingPoint<int16_t> );
		static_assert( !FloatingPoint<int32_t> );
		static_assert( !FloatingPoint<int64_t> );
	}
#pragma endregion
}