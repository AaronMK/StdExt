#include <StdExt/UID.h>

#include <StdExt/Exceptions.h>

#include <StdExt/Concurrent/Queue.h>
#include <atomic>

namespace StdExt
{
	static Concurrent::Queue<UID::uid_t> ReturnedUids;
	static std::atomic<UID::uid_t> NextId(0);

	constexpr UID::uid_t INVALID_ID = 0;

	UID::UID()
	{
		if (false == ReturnedUids.tryPop(mValue))
			mValue = ++NextId;
	}

	UID::UID(UID&& other)
	{
		mValue = other.mValue;
		other.mValue = INVALID_ID;
	}

	UID::~UID()
	{
		ReturnedUids.push(mValue);
	}

	UID& UID::operator=(UID&& other)
	{
		mValue = other.mValue;
		other.mValue = INVALID_ID;

		return *this;
	}

	UID::operator uid_t() const
	{
		if (INVALID_ID == mValue)
			throw invalid_operation("Attempting to get an invalid UID.");

		return mValue;
	}
}
