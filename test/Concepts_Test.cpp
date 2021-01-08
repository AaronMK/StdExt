#include <StdExt/Concepts.h>
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
};

void concept_test()
{

#pragma region type_transform<int>::stripped_t
	static_assert( std::is_same_v<type_transform<int>::stripped_t, int> );
	static_assert( std::is_same_v<type_transform<int*>::stripped_t, int> );
	static_assert( std::is_same_v<type_transform<const int>::stripped_t, int> );

	static_assert( std::is_same_v<type_transform<std::string>::stripped_t, std::string> );
	static_assert( std::is_same_v<type_transform<std::string*>::stripped_t, std::string> );
	static_assert( std::is_same_v<type_transform<const std::string*>::stripped_t, std::string> );

	static_assert( std::is_same_v<type_transform<std::string>::stripped_t, std::string> );
	static_assert( std::is_same_v<type_transform<std::string&>::stripped_t, std::string> );
	static_assert( std::is_same_v<type_transform<const std::string&>::stripped_t, std::string> );
#pragma endregion

#pragma region type_transform<int>::stripped_ptr_ref_t
	static_assert( std::is_same_v<type_transform<int>::stripped_ptr_ref_t, int> );
	static_assert( std::is_same_v<type_transform<int*>::stripped_ptr_ref_t, int> );
	static_assert( std::is_same_v<type_transform<const int>::stripped_ptr_ref_t, const int> );

	static_assert( std::is_same_v<type_transform<std::string>::stripped_ptr_ref_t, std::string> );
	static_assert( std::is_same_v<type_transform<std::string*>::stripped_ptr_ref_t, std::string> );
	static_assert( std::is_same_v<type_transform<const std::string*>::stripped_ptr_ref_t, const std::string> );

	static_assert( std::is_same_v<type_transform<std::string>::stripped_ptr_ref_t, std::string> );
	static_assert( std::is_same_v<type_transform<std::string&>::stripped_ptr_ref_t, std::string> );
	static_assert( std::is_same_v<type_transform<const std::string&>::stripped_ptr_ref_t, const std::string> );
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
}