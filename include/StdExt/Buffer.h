#ifndef _STD_EXT_BUFFER_H_
#define _STD_EXT_BUFFER_H_

#include "Serialize/Binary/Binary.h"

namespace StdExt
{
	class STD_EXT_EXPORT Buffer
	{
	public:
		Buffer();
		Buffer(Buffer&& other) noexcept;
		Buffer(const Buffer& other);
		Buffer(size_t size, size_t alignment = 1);

		virtual ~Buffer();

		size_t size() const;
		void resize(size_t size, size_t alignment = 0);

		const void* data() const;
		void* data();

		Buffer& operator=(Buffer&& other);
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