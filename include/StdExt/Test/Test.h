#ifndef _STD_EXT_TEST_H_
#define _STD_EXT_TEST_H_

#include <string>
#include <iostream>
#include <exception>
#include <functional>
#include <type_traits>

namespace StdExt::Test
{
	class TestFailure : public std::exception
	{
	private:
		std::string mTitle;
		std::string mMessage;

	public:
		TestFailure(const std::string& title, const std::string& message)
			: mTitle(message), mMessage(message)
		{

		}

		const char* title() const noexcept
		{
			return mTitle.c_str();
		}

		virtual const char* what() const noexcept override
		{
			return mMessage.c_str();
		}
	};

	static void raiseTestFail(const std::string& title, const std::string& message)
	{
		throw TestFailure(title, message);
	};

	/**
	 * @brief
	 *  Test for the throwing of a specified type of exception when test_func is run.
	 */
	template<typename except_t>
		requires std::is_base_of_v<std::exception, except_t>
	static void testForException(const std::string& title, const std::function<void()>& test_func)
	{
		try
		{
			test_func();
		}
		catch ( const except_t& )
		{
			std::cout << "Passed: " << title << std::endl;
			return;
		}

		raiseTestFail(title, "Expected exception not thrown.");
	}

	template<typename result_t>
		requires requires (const result_t& left, const result_t& right)
		{
			{ left == right } -> std::same_as<bool>;
		}
	static void testForResult( const std::string& title,
	                           const result_t& expected,
	                           const std::function<result_t()>& test_func )
	{
		bool passed = false;
		
		if constexpr ( FloatingPoint<result_t> )
			passed = approxEqual(test_func(), expected);
		else
			passed = (test_func() == expected);

		if ( passed )
			std::cout << "Passed: " << title << std::endl;
		else
			raiseTestFail(title, "Unexpected result.");
	}

	template<typename result_t>
		requires requires (const result_t& left, const result_t& right)
		{
			{ left == right } -> std::same_as<bool>;
		}
	static void testForResult( const std::string& title,
	                           const result_t& expected,
	                           const result_t& result)
	{
		bool passed = false;

		if constexpr ( FloatingPoint<result_t> )
			passed = approxEqual(result, expected);
		else
			passed = (result == expected);

		if ( passed )
			std::cout << "Passed: " << title << std::endl;
		else
			raiseTestFail(title, "Unexpected result.");
	}

	static void testByCheck( const std::string& title,
	                         const std::function<bool()>& check_func )
	{
		if ( check_func() )
			std::cout << "Passed: " << title << std::endl;
		else
			raiseTestFail(title, "Checking function failed.");
	}

	static void testByCheck( const std::string& title,
	                         const std::function<void()>& test_func,
	                         const std::function<bool()>& check_func )
	{
		test_func();

		if ( check_func() )
			std::cout << "Passed: " << title << std::endl;
		else
			raiseTestFail(title, "Checking function failed.");
	}
}


#endif // !_STD_EXT_TEST_H_