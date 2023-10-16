#ifndef _STD_EXT_UTILITY_H_
#define _STD_EXT_UTILITY_H_

#include <tuple>
#include <limits>
#include <random>
#include <utility>
#include <cmath>
#include <cstring>
#include <stdexcept>

#include "Concepts.h"
#include "Platform.h"
#include "Compare.h"

#include "Memory/Casting.h"

namespace StdExt
{
	template<Arithmetic T>
	T rand(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
	{
		static std::random_device rd;

		if constexpr (std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> dist(min, max);
			return dist(rd);
		}
		else
		{
			std::uniform_real_distribution<T> dist(min, max);
			return dist(rd);
		}
	}

	template<Integral T>
	static constexpr bool isPowerOf2(T number)
	{
		return (0 != number && (number & (number - 1)) == 0);
	}

	template<Integral T>
	static constexpr T nextPowerOf2(T num)
	{
		T v = num - 1;
		if constexpr ( sizeof(T) >= 1 )
		{
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
		}

		if constexpr ( sizeof(T) >= 2 )
			v |= v >> 8;

		if constexpr ( sizeof(T) >= 4 )
			v |= v >> 16;

		if constexpr ( sizeof(T) >= 8 )
			v |= v >> 32;

		return ++v;
	}

	template<Arithmetic T>
	static constexpr T nextMutltipleOf(T num, T multiple)
	{
		if constexpr ( Integral<T> )
			return (num + multiple - 1) / multiple * multiple;
		else
			return T(std::ceil(num / multiple) * multiple);
	}

	/**
	 * @brief
	 *  Computes a x b - c x d in a numerically stable fashion.
	 */
	template<Arithmetic T>
	T differenceOfProducts(T a, T b, T c, T d)
	{
		if constexpr ( FloatingPoint<T> )
		{
			T cd = c * d;
			return std::fma(a, b, -cd) + std::fma(-c, d, cd);
		}
		else
		{
			return (a * b) - (c * d);
		}
	}

	/**
	 * @brief
	 *  Arithmetic operations that will detect results outside the limits 
	 *  of the type and throw std::underflow_error or std::overflow_error
	 *  exceptions as appropriate.
	 */
	namespace Checked
	{
		template<Arithmetic T>
		T add(T left, T right)
		{
			if (right < 0)
			{
				if (left < std::numeric_limits<T>::lowest() - right)
					throw std::underflow_error("Arithmetic Underflow");
			}
			else if (left > std::numeric_limits<T>::max() - right)
			{
				throw std::overflow_error("Arithmetic Overflow");
			}

			return (left + right);
		}

		template<Arithmetic T>
		T subtract(T left, T right)
		{
			if (right < 0)
			{
				if (left > std::numeric_limits<T>::max() + right)
					throw std::overflow_error("Arithmetic Overflow");
			}
			else if (left < std::numeric_limits<T>::lowest() + right)
			{
				throw std::underflow_error("Arithmetic Underflow");
			}

			return (left - right);
		}
	}

	/**
	 * @brief
	 *  Updates dest with value, returning true if an update was necessary
	 *  or false if no change was necessary.
	 */
	template<HasNotEqual T>
	static bool update(T& dest, const T& value)
	{
		if (dest != value)
		{
			dest = value;
			return true;
		}

		return false;
	}

	/**
	 * @brief
	 *  Updates dest with value, returning true if an update was necessary
	 *  or false if no change was necessary.
	 */
	template<typename T>
		requires HasNotEqual<T> && MoveAssignable<T>
	static bool update(T& dest, T&& value)
	{
		if (dest != value)
		{
			dest = std::move(value);
			return true;
		}

		return false;
	}
	
	template<Interface T>
	class VTable final
	{
	private:
		alignas( alignof(T) ) char mTable[sizeof(T)];

		#ifdef STD_EXT_DEBUG
			T* mTablePointer;
		#endif

	public:
		VTable(const VTable&) = default;

		VTable()
		{
			memset(mTable, 0, sizeof(T));

			#ifdef STD_EXT_DEBUG
				mTablePointer = (T*)mTable;
			#endif
		}

		template<typename iface_t>
			requires Interface<T> && SuperclassOf<T, iface_t>
		void set()
		{
			new(mTable) iface_t;
		}

		void clear()
		{
			memset(mTable, 0, sizeof(T));
		}

		const T* operator->() const
		{
			return access_as<const T*>(mTable);
		}

		T* operator->()
		{
			return access_as<T*>(mTable);
		}
	};

	/**
	 * @brief
	 *  Class that calls a function at its destruction.  This is used to make sure code
	 *  is run once containing scope is complete regardless of how it is completed.
	 *
	 *  finalBlock() is a conveniece function to create these objects without having to
	 *  manually specify template parameters.
	 * 
	 * @code{.cpp}
	 *	int foo(int path)
	 *	{
	 *		auto final_block = finalBlock(
	 *			[&]()
	 *			{
	 *				std::cout << "final_block has run." << std::endl;
	 *			}
	 *		);
	 *		
	 *		if ( path < 1 )
	 *			return 0;
	 * 
	 *		if ( path < 5 )
	 *			return 1;
	 *		
	 *		throw std::exception("Invalid input.");
	 *	}
	 * @endcode
	 */
	template< CallableWith<void> func_t >
	class Finally final
	{
	private:
		func_t mFunc;

	public:
		Finally(const func_t& func)
			: mFunc(func)
		{
		}

		Finally(func_t&& func)
			: mFunc( std::move(func) )
		{
		}

		~Finally()
		{
			mFunc();
		}
	};

	/**
	 * @brief
	 *  Creates Finally object from a function object with automatic
	 *  type deduction.
	 */
	template< CallableWith<void> func_t >
	static Finally<func_t> finalBlock(const func_t& func)
	{
		return Finally<func_t>(func);
	}

	/**
	 * @brief
	 *  Creates Finally object from a function object with automatic
	 *  type deduction.
	 */
	template< CallableWith<void> func_t >
	static Finally<func_t> finalBlock(func_t&& func)
	{
		return Finally<func_t>(std::move(func));
	}
}

#endif // _STD_EXT_UTILITY_H_
