#include <StdExt/Concepts.h>

#include <StdExt/FunctionPtr.h>
#include <StdExt/Utility.h>
#include <StdExt/Type.h>

#include <string>

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

void concept_test()
{

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

	static_assert(  PointerType<const std::string*, std::string*, int**> );
	static_assert( !PointerType<const std::string&, int*, const int, std::string> );
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

	static_assert(  ConstType<const std::string*, const std::string&, const std::string> );
	static_assert( !ConstType<const std::string&, int&, const int, std::string> );
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

	static_assert( !NonConstType<const std::string*, std::string&, const std::string> );
	static_assert(  NonConstType<std::string&, int&, int, std::string> );
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

	static_assert( !ConstReferenceType<const std::string*, const std::string&, const std::string> );
	static_assert(  ConstReferenceType<const std::string&, const int&> );
#pragma endregion

#pragma region AssignableFrom
	static_assert(  AssignableFrom<const BaseClass*, SubClass*> );
	static_assert( !AssignableFrom<SubClass*, BaseClass*> );
	static_assert(  AssignableFrom<BaseClass&, SubClass> );
	static_assert( !AssignableFrom<SubClass&, BaseClass> );
	static_assert( !AssignableFrom<SubClassMoveOnly, SubClassMoveOnly> );
	static_assert(  AssignableFrom<BaseClass*, SubClassMoveOnly*> );

	static_assert(  AssignableFrom<BaseClass*, SubClassMoveOnly*, SubClass*> );
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
	static_assert(  SubclassOf<SubClassMoveOnly, BaseClass> );
	static_assert(  SubclassOf<BaseClass, BaseClass> );
	static_assert(  SuperclassOf<BaseClass, SubClassMoveOnly> );
	static_assert(  SuperclassOf<BaseClass, BaseClass> );
	static_assert(  InHeirarchyOf<SubClassMoveOnly, BaseClass> );
	static_assert(  InHeirarchyOf<BaseClass, SubClassMoveOnly> );
	static_assert(  InHeirarchyOf<BaseClass, BaseClass> );
#pragma endregion

#pragma region Callable With
	{
		auto function = [](int i, int j = 0) {};

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
	}

	static_assert(  CallableWith<FunctionPtr<int, std::string>, int, std::string>);
	static_assert(  CallableWith<FunctionPtr<int, std::string>, void, std::string>);
	static_assert( !CallableWith<FunctionPtr<int, std::string>, std::string, std::string>);
#pragma endregion
}