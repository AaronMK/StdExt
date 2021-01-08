#ifndef _STD_EXT_TYPE_H_
#define _STD_EXT_TYPE_H_

#include <typeindex>
#include <type_traits>
#include <functional>
#include <utility>
#include <memory>
#include <tuple>

namespace StdExt
{
	using float32_t = float;
	using float64_t = double;

	template<size_t index, typename... types>
	using nth_type_t = typename std::tuple_element<index, std::tuple<types...>>::type;

	/**
	 * @brief
	 *  Convenience function to get a type_index.
	 */
	template<typename T>
	std::type_index typeIndex()
	{
		return std::type_index(typeid(T));
	}

	/**
	 * @brief
	 *  Provides transformations of types.  This is a combination of grouping
	 *  functionality in the standard library and new type related functionality.  This also
	 *  provides type definitaions that are useful for templated code.
	 */
	template<typename T>
	struct type_transform
	{
	private:
		template<typename inner_t, bool more>
		struct Strip;

		template<typename inner_t>
		struct Strip<inner_t, false>
		{
			using type = inner_t;
		};

		template<typename inner_t>
		struct Strip<inner_t, true>
		{
			using stripped = std::remove_cv_t<
				std::remove_pointer_t<
					std::remove_reference_t<inner_t>
				>
			>;

			using type = typename Strip<
				stripped,
				std::is_lvalue_reference_v<stripped> ||
				std::is_rvalue_reference_v<stripped> ||
				std::is_pointer_v<stripped> ||
				std::is_const_v<stripped>
			>::type;
		};

		/////////////////////////////
		
		template<typename inner_t, bool more>
		struct StripRefPtr;

		template<typename inner_t>
		struct StripRefPtr<inner_t, false>
		{
			using type = inner_t;
		};

		template<typename inner_t>
		struct StripRefPtr<inner_t, true>
		{
			using stripped = 
				std::remove_pointer_t<
					std::remove_reference_t<inner_t>
				>;

			using type = typename StripRefPtr<
				stripped,
				std::is_reference_v <stripped> ||
				std::is_pointer_v<stripped>
			>::type;
		};

	public:

		/**
		 * @brief
		 *  True if the type does not have any constant, volitile, reference, or pointer specifiers.
		 *
		 * @code
		 * 	StdExt::Traits<int>::is_stripped;         // true
		 * 	StdExt::Traits<int*>::is_stripped;        // false
		 * 	StdExt::Traits<const int>::is_stripped;   // false
		 *
		 * 	StdExt::Traits<std::string>::is_stripped;           // true
		 * 	StdExt::Traits<std::string*>::is_stripped;          // false
		 * 	StdExt::Traits<const std::string>::is_stripped;     // false
		 *
		 * 	StdExt::Traits< std::vector<std::string> >::is_stripped; // true
		 * @endcode
		 */
		static constexpr bool is_stripped = 
			!( std::is_lvalue_reference_v<T> || std::is_pointer_v<T> ||
			   std::is_const_v<T> || std::is_rvalue_reference_v<T> );

		
		static constexpr bool is_ptr_ref_stripped = 
			!( std::is_lvalue_reference_v<T> || std::is_pointer_v<T> ||
			   std::is_rvalue_reference_v<T> );

		/**
		 * @brief
		 *  The type stripped of any constant, volitile, reference, or pointer specifiers.
		 *
		 * @code
		 * 	StdExt::Traits<int>::stripped_t;         // int
		 * 	StdExt::Traits<int*>::stripped_t;        // int
		 * 	StdExt::Traits<const int>::stripped_t;   // int
		 *
		 * 	StdExt::Traits<std::string>::stripped_t;           // std::string
		 * 	StdExt::Traits<std::string*>::stripped_t;          // std::string
		 * 	StdExt::Traits<const std::string>::stripped_t;     // std::string
		 *
		 * 	StdExt::Traits< std::vector<std::string> >::stripped_t;  // std::vector<std::string>
		 * @endcode
		 */
		using stripped_t = typename Strip<T, !is_stripped>::type;
		using stripped_ptr_ref_t = typename StripRefPtr<T, !is_ptr_ref_stripped>::type;

		using pointer_t = stripped_t*;
		using const_pointer_t = const stripped_t*;
		

		using move_t = stripped_t&&;
		using reference_t = stripped_t&;
		using const_reference_t = const stripped_t&;

		/**
		 * @brief
		 *  An arugment type that can be used to pass variable of type T as a function
		 *  parameter without a copy step, and without the ability of the function to
		 *  modify the passed variable itself.  Non-constant pointers and references
		 *  can still be used to modify the objects they reference. This basically just
		 *  amounts to structured objects being passed by constant reference.
		 *
		 * @code
		 * 	template<typename T>
		 * 	static void func(typename StdExt::Traits<T>::arg_non_copy_t arg)
		 * 	{
		 * 	}
		 * 	
		 * 	func<std::string>;         // static void func(const std::string&)
		 * 	func<std::string&>;        // static void func(std::string&)
		 * 	func<std::string&&>;       // static void func(std::string&&)
		 * 	func<std::string*>;        // static void func(std::string*)
		 * 	func<const std::string*>;  // static void func(const std::string*)
		 * 	func<int*>;                // static void func(int*)
		 * 	func<int>;                 // static void func(int)
		 * @endcode
		 */
		using arg_non_copy_t = typename std::conditional_t<std::is_class_v<T>, const T&, T>;

		/**
		 * @brief
		 *  Provides a type to T that can be trivially be passed into a function without
		 *  calling a copy constructor, and protecting the parameter and what it references from
		 *  modification within the function.  For value types, this will simply be the value
		 *  itself.  For structured types and references, this will be constant reference.
		 *  For pointer types, this will be a constant pointer.
		 *
		 * @code
		 * 	template<typename T>
		 * 	static void func(typename StdExt::Traits<T>::arg_non_copy_t arg)
		 * 	{
		 * 	}
		 * 	
		 * 	func<std::string>;         // static void func(const std::string&)
		 * 	func<std::string&>;        // static void func(const std::string&)
		 * 	func<std::string&&>;       // static void func(const std::string&)
		 * 	func<std::string*>;        // static void func(const std::string*)
		 * 	func<const std::string*>;  // static void func(const std::string*)
		 * 	func<int*>;                // static void func(const int*)
		 * 	func<int>;                 // static void func(int)
		 * @endcode
		 */
		using arg_non_copy_const_t =
			typename std::conditional_t<
				std::is_class_v<T> || std::is_reference_v<T>,
				const_reference_t,
				std::conditional_t<
					std::is_pointer_v<T>,
					std::add_const_t<T>,
					T
				>
			>;
	};
}

#endif // !_STD_EXT_TYPE_H_