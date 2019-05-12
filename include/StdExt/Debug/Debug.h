#ifndef _STD_EXT_DEBUG_H_
#define _STD_EXT_DEBUG_H_

#ifdef _DEBUG
#include <vector>

#include "../Type.h"

#endif

namespace StdExt::Debug
{
#ifdef _DEBUG
	template<typename T>
	class ArrayWatch : std::conditional_t<Traits<T>::is_pointer, std::vector<T>, std::vector<T*>>
	{
	private:
		T*& mArray;
		size_t& mSize;

	public:
		ArrayWatch(T*& _array, size_t& _size)
			: mArray(_array), mSize(_size)
		{
		}

		void updateView()
		{
			resize(mSize);
			for (size_t i = 0; i < mSize; ++i)
			{
				if constexpr (Traits<T>::is_pointer)
					(*this)[i] = mArray[i];
				else
					(*this)[i] = &mArray[i];
			}
		}
	};
#else
	template<typename T>
	class ArrayWatch
	{
	public:
		ArrayWatch(T*& _array, size_t& _Size) {}
		void updateView() {}
	};
#endif
}

#endif // !_STD_EXT_DEBUG_H_
