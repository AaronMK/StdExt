#ifndef _STD_EXT_FUNCTION_PTR_H_
#define _STD_EXT_FUNCTION_PTR_H_

#include <type_traits>
#include <algorithm>

#include "Exceptions.h"
#include "Memory.h"
#include "Utility.h"

namespace StdExt
{
	/**
	 * @brief
	 *  Convenience definition of a member function pointer to avoid syntax that can
	 *  be messy and hard to remember.
	 *
	 * @tparam class_t
	 *  The class of the member function.
	 *
	 * @tparam return_t
	 *  return_t The return type of the function.
	 *
	 * @tparam args_t
	 *  The types of the arguments.
	 */
	template<Class class_t, typename return_t, typename ...args_t>
	using MemberFunctionPtr = return_t(class_t::*)(args_t...);

	/**
	 * @brief
	 *  Convenience definition of a const member function pointer to avoid syntax
	 *  that can be messy and hard to remember.
	 *
	 * @tparam class_t
	 *  The class of the member function.
	 *
	 * @tparam return_t
	 *  return_t The return type of the function.
	 *
	 * @tparam args_t
	 *  The types of the arguments.
	 */
	template<Class class_t, typename return_t, typename ...args_t>
	using ConstMemberFunctionPtr = return_t(class_t::*)(args_t...) const;

	/**
	 * @brief
	 *  Convenience definition of a static function pointer to avoid syntax
	 *  that can be messy and hard to remember.
	 *
	 * @tparam return_t
	 *  return_t The return type of the function.
	 *
	 * @tparam args_t
	 *  The types of the arguments.
	 */
	template<typename return_t, typename ...args_t>
	using StaticFunctionPtr = return_t(*)(args_t...);
}

#endif //!_STD_EXT_FUNCTION_PTR_H_