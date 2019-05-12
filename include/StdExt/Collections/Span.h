#ifndef _STD_EXT_COLLECTIONS_SPAN_H_
#define _STD_EXT_COLLECTIONS_SPAN_H_

#include "../Debug/Debug.h"
#include "../Platform.h"

#include <stdexcept>

namespace StdExt::Collections
{
	/**
	 * @brief
	 *  Provides a range checked wrapper around access to an allocation of
	 *  objects.
	 */
	template<typename T>
	class Span
	{
	private:
		T* mObjs;
		size_t mSize;

	public:
		Span(const Span<T>& other) = default;
		Span<T>& operator=(const Span<T>& other) = default;

		Span() noexcept
		{
			mObjs = nullptr;
			mSize = 0;
		}

		Span(Span<T>&& other) noexcept
		{
			mSize = other.mSize;
			mObjs = other.mObjs;

			other.mSize = 0;
			other.mObjs = nullptr;
		}

		Span(T* objs, size_t size) noexcept
		{
			mSize = size;
			mObjs = objs;
		}

		Span<T>& operator=(Span<T>&& other) noexcept
		{
			mSize = other.mSize;
			mObjs = other.mObjs;

			other.mSize = 0;
			other.mObjs = nullptr;

			return *this;
		}

		bool operator==(const Span<T>& other)
		{
			return (mSize == other.mSize && mObjs == other.mObjs);
		}

		bool operator!=(const Span<T>& other)
		{
			return (mSize != other.mSize || mObjs != other.mObjs);
		}

		T& operator[](size_t index)
		{
			if ( Platform::isDebug && index >= mSize )
				throw std::out_of_range("Access outside of array bounds detected.");

			return mObjs[index];
		}

		const T& operator[](size_t index) const
		{
			if ( Platform::isDebug && index >= mSize )
				throw std::out_of_range("Access outside of array bounds detected.");

			return mObjs[index];
		}

		operator bool() const
		{
			return (nullptr != mObjs);
		}

		size_t size() const
		{
			return mSize;
		}

		T* data()
		{
			return mObjs;
		}

		const T* data() const
		{
			return mObjs;
		}

		Span<T> subSpan(size_t start, size_t count) const
		{
			if (Platform::isDebug)
			{
				try
				{
					if ( Checked::add(start, count) > mSize )
						throw std::out_of_range("Sub span goes outside original span.");
				}
				catch (const std::overflow_error&)
				{
					throw std::out_of_range("Sub span goes outside original span.");
				}
			}
			
			if (nullptr == mObjs || 0 == count)
				return Span<T>();

			return Span<T>(&mObjs[start], count);
		}

		Span<T> subSpan(size_t start)
		{
			return subSpan(start, mSize - start);
		}

		static Debug::ArrayWatch<T> watch(Span<T>& span)
		{
			Debug::ArrayWatch<T> ret = Debug::ArrayWatch<T>(span.mObjs, span.mSize);
			ret.updateView();

			return ret;
		}
	};
}

#endif // !_STD_EXT_COLLECTIONS_SPAN_H_