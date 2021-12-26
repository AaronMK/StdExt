#include <StdExt/Vec.h>

#include <StdExt/Streams/ByteStream.h>

namespace StdExt
{
	namespace Serialize::Binary
	{
		template<typename T>
		void readVec2(ByteStream* stream, Vec2<T>* out)
		{
			(*out)[0] = read<T>(stream);
			(*out)[1] = read<T>(stream);
		}

		template<typename T>
		void writeVec2(ByteStream* stream, const Vec2<T>& val)
		{
			write<T>(stream, val[0]);
			write<T>(stream, val[1]);
		}

		template<>
		void read(ByteStream* stream, Vec2<bool>* out)
		{
			readVec2<bool>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<bool>& val)
		{
			writeVec2<bool>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<uint8_t>* out)
		{
			readVec2<uint8_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec2<uint8_t>& val)
		{
			writeVec2<uint8_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<uint16_t>* out)
		{
			readVec2<uint16_t>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<uint16_t>& val)
		{
			writeVec2<uint16_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<uint32_t>* out)
		{
			readVec2<uint32_t>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<uint32_t>& val)
		{
			writeVec2<uint32_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<uint64_t>* out)
		{
			readVec2<uint64_t>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<uint64_t>& val)
		{
			writeVec2<uint64_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<int8_t>* out)
		{
			readVec2<int8_t>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<int8_t>& val)
		{
			writeVec2<int8_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<int16_t>* out)
		{
			readVec2<int16_t>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<int16_t>& val)
		{
			writeVec2<int16_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<int32_t>* out)
		{
			readVec2<int32_t>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<int32_t>& val)
		{
			writeVec2<int32_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<int64_t>* out)
		{
			readVec2<int64_t>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<int64_t>& val)
		{
			writeVec2<int64_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<float32_t>* out)
		{
			readVec2<float32_t>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<float32_t>& val)
		{
			writeVec2<float32_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec2<float64_t>* out)
		{
			readVec2<float64_t>(stream, out);
		}
		
		template<>
		void write(ByteStream* stream, const Vec2<float64_t>& val)
		{
			writeVec2<float64_t>(stream, val);
		}

		/////////////////////////////////////////////////

		template<typename T>
		void readVec3(ByteStream* stream, Vec3<T>* out)
		{
			(*out)[0] = read<T>(stream);
			(*out)[1] = read<T>(stream);
			(*out)[2] = read<T>(stream);
		}

		template<typename T>
		void writeVec3(ByteStream* stream, const Vec3<T>& val)
		{
			write<T>(stream, val[0]);
			write<T>(stream, val[1]);
			write<T>(stream, val[2]);
		}

		template<>
		void read(ByteStream* stream, Vec3<bool>* out)
		{
			readVec3<bool>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<bool>& val)
		{
			writeVec3<bool>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<uint8_t>* out)
		{
			readVec3<uint8_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<uint8_t>& val)
		{
			writeVec3<uint8_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<uint16_t>* out)
		{
			readVec3<uint16_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<uint16_t>& val)
		{
			writeVec3<uint16_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<uint32_t>* out)
		{
			readVec3<uint32_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<uint32_t>& val)
		{
			writeVec3<uint32_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<uint64_t>* out)
		{
			readVec3<uint64_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<uint64_t>& val)
		{
			writeVec3<uint64_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<int8_t>* out)
		{
			readVec3<int8_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<int8_t>& val)
		{
			writeVec3<int8_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<int16_t>* out)
		{
			readVec3<int16_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<int16_t>& val)
		{
			writeVec3<int16_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<int32_t>* out)
		{
			readVec3<int32_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<int32_t>& val)
		{
			writeVec3<int32_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<int64_t>* out)
		{
			readVec3<int64_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<int64_t>& val)
		{
			writeVec3<int64_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<float32_t>* out)
		{
			readVec3<float32_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<float32_t>& val)
		{
			writeVec3<float32_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec3<float64_t>* out)
		{
			readVec3<float64_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec3<float64_t>& val)
		{
			writeVec3<float64_t>(stream, val);
		}

		/////////////////////////////////////////////////

		template<typename T>
		void readVec4(ByteStream* stream, Vec4<T>* out)
		{
			(*out)[0] = read<T>(stream);
			(*out)[1] = read<T>(stream);
			(*out)[2] = read<T>(stream);
			(*out)[3] = read<T>(stream);
		}

		template<typename T>
		void writeVec4(ByteStream* stream, const Vec4<T>& val)
		{
			write<T>(stream, val[0]);
			write<T>(stream, val[1]);
			write<T>(stream, val[2]);
			write<T>(stream, val[3]);
		}

		template<>
		void read(ByteStream* stream, Vec4<bool>* out)
		{
			readVec4<bool>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<bool>& val)
		{
			writeVec4<bool>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<uint8_t>* out)
		{
			readVec4<uint8_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<uint8_t>& val)
		{
			writeVec4<uint8_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<uint16_t>* out)
		{
			readVec4<uint16_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<uint16_t>& val)
		{
			writeVec4<uint16_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<uint32_t>* out)
		{
			readVec4<uint32_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<uint32_t>& val)
		{
			writeVec4<uint32_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<uint64_t>* out)
		{
			readVec4<uint64_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<uint64_t>& val)
		{
			writeVec4<uint64_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<int8_t>* out)
		{
			readVec4<int8_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<int8_t>& val)
		{
			writeVec4<int8_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<int16_t>* out)
		{
			readVec4<int16_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<int16_t>& val)
		{
			writeVec4<int16_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<int32_t>* out)
		{
			readVec4<int32_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<int32_t>& val)
		{
			writeVec4<int32_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<int64_t>* out)
		{
			readVec4<int64_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<int64_t>& val)
		{
			writeVec4<int64_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<float32_t>* out)
		{
			readVec4<float32_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<float32_t>& val)
		{
			writeVec4<float32_t>(stream, val);
		}

		template<>
		void read(ByteStream* stream, Vec4<float64_t>* out)
		{
			readVec4<float64_t>(stream, out);
		}

		template<>
		void write(ByteStream* stream, const Vec4<float64_t>& val)
		{
			writeVec4<float64_t>(stream, val);
		}
	}

	namespace Serialize::XML
	{
		template<typename T>
		void readVec2(const Element& element, Vec2<T>* out)
		{
			auto strings = element.text().split(", ");
			(*out)[0] = Serialize::Text::read<T>(strings[0]);
			(*out)[1] = Serialize::Text::read<T>(strings[1]);
		}

		template<typename T>
		void writeVec2(Element& element, const Vec2<T>& val)
		{
			std::array<String, 2> strings;
			strings[0] = Serialize::Text::write(val[0]);
			strings[1] = Serialize::Text::write(val[1]);

			element.setText( String::join(strings, ", "));
		}

		template<>
		void read(const Element& element, Vec2<bool>* out)
		{
			readVec2<bool>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<bool>& val)
		{
			writeVec2<bool>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<uint8_t>* out)
		{
			readVec2<uint8_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<uint8_t>& val)
		{
			writeVec2<uint8_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<uint16_t>* out)
		{
			readVec2<uint16_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<uint16_t>& val)
		{
			writeVec2<uint16_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<uint32_t>* out)
		{
			readVec2<uint32_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<uint32_t>& val)
		{
			writeVec2<uint32_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<uint64_t>* out)
		{
			readVec2<uint64_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<uint64_t>& val)
		{
			writeVec2<uint64_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<int8_t>* out)
		{
			readVec2<int8_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<int8_t>& val)
		{
			writeVec2<int8_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<int16_t>* out)
		{
			readVec2<int16_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<int16_t>& val)
		{
			writeVec2<int16_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<int32_t>* out)
		{
			readVec2<int32_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<int32_t>& val)
		{
			writeVec2<int32_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<int64_t>* out)
		{
			readVec2<int64_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<int64_t>& val)
		{
			writeVec2<int64_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<float32_t>* out)
		{
			readVec2<float32_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<float32_t>& val)
		{
			writeVec2<float32_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec2<float64_t>* out)
		{
			readVec2<float64_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec2<float64_t>& val)
		{
			writeVec2<float64_t>(element, val);
		}

		/////////////////////////////////////////////////

		template<typename T>
		void readVec3(const Element& element, Vec3<T>* out)
		{
			auto strings = element.text().split(", ");
			(*out)[0] = Serialize::Text::read<T>(strings[0]);
			(*out)[1] = Serialize::Text::read<T>(strings[1]);
			(*out)[2] = Serialize::Text::read<T>(strings[2]);
		}

		template<typename T>
		void writeVec3(Element& element, const Vec3<T>& val)
		{
			std::array<String, 3> strings;
			strings[0] = Serialize::Text::write(val[0]);
			strings[1] = Serialize::Text::write(val[1]);
			strings[2] = Serialize::Text::write(val[2]);

			element.setText(String::join(strings, ", "));
		}

		template<>
		void read(const Element& element, Vec3<bool>* out)
		{
			readVec3<bool>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<bool>& val)
		{
			writeVec3<bool>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<uint8_t>* out)
		{
			readVec3<uint8_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<uint8_t>& val)
		{
			writeVec3<uint8_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<uint16_t>* out)
		{
			readVec3<uint16_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<uint16_t>& val)
		{
			writeVec3<uint16_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<uint32_t>* out)
		{
			readVec3<uint32_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<uint32_t>& val)
		{
			writeVec3<uint32_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<uint64_t>* out)
		{
			readVec3<uint64_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<uint64_t>& val)
		{
			writeVec3<uint64_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<int8_t>* out)
		{
			readVec3<int8_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<int8_t>& val)
		{
			writeVec3<int8_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<int16_t>* out)
		{
			readVec3<int16_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<int16_t>& val)
		{
			writeVec3<int16_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<int32_t>* out)
		{
			readVec3<int32_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<int32_t>& val)
		{
			writeVec3<int32_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<int64_t>* out)
		{
			readVec3<int64_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<int64_t>& val)
		{
			writeVec3<int64_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<float32_t>* out)
		{
			readVec3<float32_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<float32_t>& val)
		{
			writeVec3<float32_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec3<float64_t>* out)
		{
			readVec3<float64_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec3<float64_t>& val)
		{
			writeVec3<float64_t>(element, val);
		}

		/////////////////////////////////////////////////

		template<typename T>
		void readVec4(const Element& element, Vec4<T>* out)
		{
			auto strings = element.text().split(", ");
			(*out)[0] = Serialize::Text::read<T>(strings[0]);
			(*out)[1] = Serialize::Text::read<T>(strings[1]);
			(*out)[2] = Serialize::Text::read<T>(strings[2]);
			(*out)[3] = Serialize::Text::read<T>(strings[3]);
		}

		template<typename T>
		void writeVec4(Element& element, const Vec4<T>& val)
		{
			std::array<String, 4> strings;
			strings[0] = Serialize::Text::write(val[0]);
			strings[1] = Serialize::Text::write(val[1]);
			strings[2] = Serialize::Text::write(val[2]);
			strings[3] = Serialize::Text::write(val[3]);

			element.setText(String::join(strings, ", "));
		}

		template<>
		void read(const Element& element, Vec4<bool>* out)
		{
			readVec4<bool>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<bool>& val)
		{
			writeVec4<bool>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<uint8_t>* out)
		{
			readVec4<uint8_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<uint8_t>& val)
		{
			writeVec4<uint8_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<uint16_t>* out)
		{
			readVec4<uint16_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<uint16_t>& val)
		{
			writeVec4<uint16_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<uint32_t>* out)
		{
			readVec4<uint32_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<uint32_t>& val)
		{
			writeVec4<uint32_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<uint64_t>* out)
		{
			readVec4<uint64_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<uint64_t>& val)
		{
			writeVec4<uint64_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<int8_t>* out)
		{
			readVec4<int8_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<int8_t>& val)
		{
			writeVec4<int8_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<int16_t>* out)
		{
			readVec4<int16_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<int16_t>& val)
		{
			writeVec4<int16_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<int32_t>* out)
		{
			readVec4<int32_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<int32_t>& val)
		{
			writeVec4<int32_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<int64_t>* out)
		{
			readVec4<int64_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<int64_t>& val)
		{
			writeVec4<int64_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<float32_t>* out)
		{
			readVec4<float32_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<float32_t>& val)
		{
			writeVec4<float32_t>(element, val);
		}

		template<>
		void read(const Element& element, Vec4<float64_t>* out)
		{
			readVec4<float64_t>(element, out);
		}

		template<>
		void write(Element& element, const Vec4<float64_t>& val)
		{
			writeVec4<float64_t>(element, val);
		}
	}
}