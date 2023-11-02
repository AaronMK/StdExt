#ifndef _STD_EXT_CHRONO_DURATION_H_
#define _STD_EXT_CHRONO_DURATION_H_

#include "../StdExt.h"

#include <chrono>

namespace StdExt::Chrono
{
	/**
	 * @brief
	 *  Duration class that extends std::chono::duration with conversion operators.
	 */
	template<typename rep_t, typename period_t = std::ratio<1>>
	class Duration : public std::chrono::duration<rep_t, period_t>
	{
	public:
		using base_t = std::chrono::duration<rep_t, period_t>;

		Duration(const Duration&) = default;
		Duration(Duration&&) = default;

		Duration& operator=(const Duration&) = default;
		Duration& operator=(Duration&&) = default;

		Duration() = default;

		constexpr Duration(rep_t cnt)
			: base_t(cnt)
		{
		}

		template<typename other_rep_t, typename other_period_t>
		constexpr Duration(const std::chrono::duration<other_rep_t, other_period_t>& other)
			: base_t( std::chrono::duration_cast<base_t>(other) )
		{
		}

		template<typename other_rep_t, typename other_period_t>
		constexpr operator std::chrono::duration<other_rep_t, other_period_t>() const
		{
			return
				std::chrono::duration_cast<
					std::chrono::duration<other_rep_t, other_period_t>
				>(*this);
		}
	};

	/**
	 * @brief
	 *  Wraps StdExt Duration functionality around a standard library duration.
	 */
	template<typename rep_t, typename period_t>
	constexpr auto makeDuration(const std::chrono::duration<rep_t, period_t>& std_duration)
	{
		return Duration<rep_t, period_t>(std_duration);
	}

	using Nanoseconds  = Duration<double, std::nano>;
	using Microseconds = Duration<double, std::micro>;
	using Milliseconds = Duration<double, std::milli>;
	using Seconds      = Duration<double>;
	using Minutes      = Duration<double, std::ratio<60>>;
	using Hours        = Duration<double, std::ratio<3600>>;
	using Days         = Duration<double, std::ratio<86400>>;
	using Weeks        = Duration<double, std::ratio<604800>>;
	using Months       = Duration<double, std::ratio<2629746>>;
	using Years        = Duration<double, std::ratio<31556952>>;
}

#endif // ! _STD_EXT_CHRONO_DURATION_H_