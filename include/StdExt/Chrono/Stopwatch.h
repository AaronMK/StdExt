#ifndef _STD_EXT_CHRONO_STOPWATCH_H_
#define _STD_EXT_CHRONO_STOPWATCH_H_

#include "../StdExt.h"

#include <chrono>

namespace StdExt::Chrono
{
	class Stopwatch final
	{
	public:
		using time_point_t = std::chrono::steady_clock::time_point;
		using duration_t   = std::chrono::steady_clock::duration;
		using clock_t      = std::chrono::steady_clock;

		Stopwatch()
		{
			reset();
		}

		~Stopwatch()
		{
		}

		void start()
		{
			if ( !mIsRunning )
			{
				mIsRunning = true;
				mStartTime = clock_t::now();
			}
		}

		void stop()
		{
			if ( mIsRunning )
			{
				mAccumulated += (clock_t::now() - mStartTime);
				mIsRunning = false;
			}
		}

		void reset()
		{
			mStartTime = time_point_t();
			mAccumulated = duration_t(0);
			mIsRunning = false;
		}

		duration_t time() const
		{
			return ( mIsRunning ) ?
				mAccumulated + (clock_t::now() - mStartTime) :
				mAccumulated;
		}

	private:
		time_point_t mStartTime;
		duration_t   mAccumulated;
		bool         mIsRunning;
	};
}

#endif // !_STD_EXT_CHRONO_STOPWATCH_H_