#ifndef _STD_EXT_BUFFER_H_
#define _STD_EXT_BUFFER_H_

#include "Serialize/Binary/Binary.h"

namespace StdExt
{
	/**
	 * @brief
	 *  Class that wraps a buffer object, handling alignment details, ownership of the memory,
	 *  and movement/copying of data as needed.
	 */
	class STD_EXT_EXPORT Buffer
	{
	public:
		
		/**
		 * @brief
		 *  Creates an empty buffer without any allocations.
		 */
		Buffer();

		/**
		 * @brief
		 *  Transfers ownership of allocated memory from other.
		 */
		Buffer(Buffer&& other) noexcept;

		/**
		 * @brief
		 *  Creates and allocates buffer with the same size and alignment as other,
		 *  copying other's data into the new buffer.
		 */
		Buffer(const Buffer& other);
		
		/**
		 * @brief
		 *  Creates and allocates an uninitialized buffer with the given size and alignment.
		 */
		Buffer(size_t size, size_t alignment = 1);

		/**
		 * @brief
		 *  Destructor deallocates memory owned by the buffer.
		 */
		virtual ~Buffer();

		/**
		 * @brief
		 *  Gets the total size in bytes allocated for the buffer.
		 */
		size_t size() const;

		/**
		 * @brief
		 *  Resizes the buffer retaining existing data.  If the buffer is smaller than
		 *  the current size, data will be truncated.  If alignment is 0, the current
		 *  alignment will be used.
		 */
		void resize(size_t size, size_t alignment = 0);

		/**
		 * @brief
		 *  Gets a pointer to the beginning of the buffer.
		 */
		const void* data() const;
		
		/**
		 * @brief
		 *  Gets a pointer to the beginning of the buffer.
		 */
		void* data();

		/**
		 * @brief
		 *  Destroys any data in this buffer, and takes onwership of the data in other.
		 *  When completed, the allocated memory will be the same size as the starting
		    size for other, and other will contain no data.
		 */
		Buffer& operator=(Buffer&& other);

		/**
		 * @brief
		 *  Destroys any data in this buffer, and copies the data from other into
		 *  this buffer. 
		 */
		Buffer& operator=(const Buffer& other);

	private:
		size_t mSize;
		size_t mAlignment;
		void* mBuffer;
	};
}

namespace StdExt::Serialize::Binary
{
	template<>
	STD_EXT_EXPORT void read<StdExt::Buffer>(ByteStream* stream, StdExt::Buffer* out);

	template<>
	STD_EXT_EXPORT void write<StdExt::Buffer>(ByteStream* stream, const StdExt::Buffer& val);
}

#endif // _STD_EXT_BUFFER_H_