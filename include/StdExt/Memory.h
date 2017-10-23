#ifndef _STD_EXT_MEMORY_H_
#define _STD_EXT_MEMORY_H_

#include "StdExt.h"

#ifndef NOMINMAX
#	define NOMINMAX
#endif

#include "Exceptions.h"

#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <memory>


namespace StdExt
{
	template<typename T, typename U>
	T* align(U*& ptr, std::size_t* space)
	{
		void* vPtr = ptr;

		if (std::align(alignof(T), sizeof(T), vPtr, *space))
		{
			ptr = reinterpret_cast<U*>(vPtr);
			return reinterpret_cast<T*>(vPtr);
		}

		return nullptr;
	}

	template<typename T, typename U>
	T* align(U*& ptr, std::size_t space)
	{
		return align<T>(ptr, &space);
	}

	template<typename... _Types>
	struct AlignedBlockSize;

	template<>
	struct AlignedBlockSize<>
	{
		static const size_t value = 1;
	};

	/**
	 * @brief
	 *  The value member of this struct contains the smallest block size
	 *  for which std::align would be guarenteed at succeeding for all
	 *  type parameters.
	 */
	template<typename _This, typename... _Rest>
	struct AlignedBlockSize<_This, _Rest...>
	{
		static constexpr size_t value = std::max(
			sizeof(_This) + alignof(_This) - 1,
			AlignedBlockSize<_Rest...>::value);
	};

	template<typename... _types>
	constexpr size_t AlignedBlockSize_v = AlignedBlockSize<_types...>::value;

	/**
	 * @brief
	 *  Returns the original value of ptr after setting the ptr reference passed to nullptr.
	 */
	template<typename T>
	T* movePtr(T*& ptr)
	{
		T* ret = ptr;
		ptr = nullptr;

		return ret;
	}

	template<typename ptr_t, typename tag_t>
	class TaggedPtr
	{
		static_assert(sizeof(tag_t) <= 2);

		static const uint64_t TAG_MASK = 0xFFFF000000000000;
		static const uint64_t PTR_MASK = 0x0000FFFFFFFFFFFF;

		uint64_t mData;

	public:
		TaggedPtr<ptr_t, tag_t>(const TaggedPtr<ptr_t, tag_t>& other) = default;
		TaggedPtr<ptr_t, tag_t>& operator=(const TaggedPtr<ptr_t, tag_t>& other) = default;

		TaggedPtr()
		{
			mData = 0;
		}

		tag_t tag() const
		{
			return (tag_t)((mData & TAG_MASK) >> 48);
		}

		ptr_t* ptr() const
		{
			return (ptr_t*)(mData & PTR_MASK);
		}

		operator ptr_t*() const
		{
			return ptr();
		}

		operator tag_t() const
		{
			return tag();
		}

		ptr_t operator*() const
		{
			return *ptr();
		}

		ptr_t* operator->() const
		{
			return ptr();
		}

		ptr_t* operator=(ptr_t* _ptr)
		{
			mData |= ((uint64_t)_ptr & PTR_MASK);
			return _ptr;
		}

		tag_t operator=(tag_t _tag)
		{
			uint64_t temp;
			memcpy_s(&temp, sizeof(temp), &_tag, sizeof(tag_t));

			mData |= ((temp << 48) & TAG_MASK);
			return _tag;
		}
	};
}

#endif // _STD_EXT_MEMORY_H_