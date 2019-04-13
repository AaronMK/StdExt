#ifndef _STD_EXT_DEBUG_H_

#ifdef _DEBUG
#include <vector>

#include "../Type.h"
#endif

namespace StdExt::Debug
{
#ifdef _DEBUG
	template<typename T>
	class ArrayWatch
	{
	private:
		using vec_t = std::conditional_t<Traits<T>::is_pointer, std::vector<T>, std::vector<T*>>;

		T*& mArray;
		size_t& mSize;

		vec_t mView;

	public:
		ArrayWatch(T*& _array, size_t& _size)
			: mArray(_array), mSize(_size)
		{
		}

		void updateView()
		{
			mView.resize(mSize);
			for (size_t i = 0; i < mSize; ++i)
			{
				if constexpr (Traits<T>::is_pointer)
					mView[i] = mArray[i];
				else
					mView[i] = &mArray[i];
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
