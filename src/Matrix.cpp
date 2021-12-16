#include <StdExt/Matrix.h>

namespace StdExt
{
	namespace Serialize::Binary
	{
		template<Arithmetic T>
		void readMatrix2(ByteStream* stream, Matrix2x2<T>* out)
		{
			Matrix2x2<T>& out_ref = access_as<Matrix2x2<T>&>(out);

			read< Vec2<T> >(stream, &out_ref[0]);
			read< Vec2<T> >(stream, &out_ref[1]);
		}

		template<Arithmetic T>
		void writeMatrix2(ByteStream* stream, const Matrix2x2<T>& val)
		{
			write< Vec2<T> >(stream, val[0]);
			write< Vec2<T> >(stream, val[1]);
		}

		template<> void read(ByteStream* stream, Matrix2x2<uint8_t>* out)
		{
			readMatrix2<uint8_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<uint8_t>& val)
		{
			writeMatrix2<uint8_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix2x2<uint16_t>* out)
		{
			readMatrix2<uint16_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<uint16_t>& val)
		{
			writeMatrix2<uint16_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix2x2<uint32_t>* out)
		{
			readMatrix2<uint32_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<uint32_t>& val)
		{
			writeMatrix2<uint32_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix2x2<uint64_t>* out)
		{
			readMatrix2<uint64_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<uint64_t>& val)
		{
			writeMatrix2<uint64_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix2x2<int8_t>* out)
		{
			readMatrix2<int8_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<int8_t>& val)
		{
			writeMatrix2<int8_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix2x2<int16_t>* out)
		{
			readMatrix2<int16_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<int16_t>& val)
		{
			writeMatrix2<int16_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix2x2<int32_t>* out)
		{
			readMatrix2<int32_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<int32_t>& val)
		{
			writeMatrix2<int32_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix2x2<int64_t>* out)
		{
			readMatrix2<int64_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<int64_t>& val)
		{
			writeMatrix2<int64_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix2x2<float32_t>* out)
		{
			readMatrix2<float32_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<float32_t>& val)
		{
			writeMatrix2<float32_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix2x2<float64_t>* out)
		{
			readMatrix2<float64_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix2x2<float64_t>& val)
		{
			writeMatrix2<float64_t>(stream, val);
		}

		//////////////////////////////////////

		template<Arithmetic T>
		void readMatrix3(ByteStream* stream, Matrix3x3<T>* out)
		{
			Matrix3x3<T>& out_ref = access_as<Matrix3x3<T>&>(out);

			read< Vec3<T> >(stream, &out_ref[0]);
			read< Vec3<T> >(stream, &out_ref[1]);
			read< Vec3<T> >(stream, &out_ref[2]);
		}

		template<Arithmetic T>
		void writeMatrix3(ByteStream* stream, const Matrix3x3<T>& val)
		{
			write< Vec3<T> >(stream, val[0]);
			write< Vec3<T> >(stream, val[1]);
			write< Vec3<T> >(stream, val[2]);
		}

		template<> void read(ByteStream* stream, Matrix3x3<uint8_t>* out)
		{
			readMatrix3<uint8_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<uint8_t>& val)
		{
			writeMatrix3<uint8_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix3x3<uint16_t>* out)
		{
			readMatrix3<uint16_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<uint16_t>& val)
		{
			writeMatrix3<uint16_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix3x3<uint32_t>* out)
		{
			readMatrix3<uint32_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<uint32_t>& val)
		{
			writeMatrix3<uint32_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix3x3<uint64_t>* out)
		{
			readMatrix3<uint64_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<uint64_t>& val)
		{
			writeMatrix3<uint64_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix3x3<int8_t>* out)
		{
			readMatrix3<int8_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<int8_t>& val)
		{
			writeMatrix3<int8_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix3x3<int16_t>* out)
		{
			readMatrix3<int16_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<int16_t>& val)
		{
			writeMatrix3<int16_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix3x3<int32_t>* out)
		{
			readMatrix3<int32_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<int32_t>& val)
		{
			writeMatrix3<int32_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix3x3<int64_t>* out)
		{
			readMatrix3<int64_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<int64_t>& val)
		{
			writeMatrix3<int64_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix3x3<float32_t>* out)
		{
			readMatrix3<float32_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<float32_t>& val)
		{
			writeMatrix3<float32_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix3x3<float64_t>* out)
		{
			readMatrix3<float64_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix3x3<float64_t>& val)
		{
			writeMatrix3<float64_t>(stream, val);
		}

		//////////////////////////////////////

		template<Arithmetic T>
		void readMatrix4(ByteStream* stream, Matrix4x4<T>* out)
		{
			Matrix4x4<T>& out_ref = access_as<Matrix4x4<T>&>(out);

			read< Vec4<T> >(stream, &out_ref[0]);
			read< Vec4<T> >(stream, &out_ref[1]);
			read< Vec4<T> >(stream, &out_ref[2]);
			read< Vec4<T> >(stream, &out_ref[3]);
		}

		template<Arithmetic T>
		void writeMatrix4(ByteStream* stream, const Matrix4x4<T>& val)
		{
			write< Vec4<T> >(stream, val[0]);
			write< Vec4<T> >(stream, val[1]);
			write< Vec4<T> >(stream, val[2]);
			write< Vec4<T> >(stream, val[3]);
		}

		template<> void read(ByteStream* stream, Matrix4x4<uint8_t>* out)
		{
			readMatrix4<uint8_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<uint8_t>& val)
		{
			writeMatrix4<uint8_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix4x4<uint16_t>* out)
		{
			readMatrix4<uint16_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<uint16_t>& val)
		{
			writeMatrix4<uint16_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix4x4<uint32_t>* out)
		{
			readMatrix4<uint32_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<uint32_t>& val)
		{
			writeMatrix4<uint32_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix4x4<uint64_t>* out)
		{
			readMatrix4<uint64_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<uint64_t>& val)
		{
			writeMatrix4<uint64_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix4x4<int8_t>* out)
		{
			readMatrix4<int8_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<int8_t>& val)
		{
			writeMatrix4<int8_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix4x4<int16_t>* out)
		{
			readMatrix4<int16_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<int16_t>& val)
		{
			writeMatrix4<int16_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix4x4<int32_t>* out)
		{
			readMatrix4<int32_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<int32_t>& val)
		{
			writeMatrix4<int32_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix4x4<int64_t>* out)
		{
			readMatrix4<int64_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<int64_t>& val)
		{
			writeMatrix4<int64_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix4x4<float32_t>* out)
		{
			readMatrix4<float32_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<float32_t>& val)
		{
			writeMatrix4<float32_t>(stream, val);
		}

		template<> void read(ByteStream* stream, Matrix4x4<float64_t>* out)
		{
			readMatrix4<float64_t>(stream, out);
		}

		template<> void write(ByteStream* stream, const Matrix4x4<float64_t>& val)
		{
			writeMatrix4<float64_t>(stream, val);
		}
	}

	namespace Serialize::XML
	{
		template<Arithmetic T>
		void readMatrix2(const Element& element, Matrix2x2<T>* out)
		{
			Matrix2x2<T>& out_ref = access_as<Matrix2x2<T>&>(out);

			element.getChild(SL("Col0"), &out_ref[0]);
			element.getChild(SL("Col1"), &out_ref[1]);
		}

		template<Arithmetic T>
		void writeMatrix2(Element& element, const Matrix2x2<T>& val)
		{
			element.addChild(SL("Col0"), val[0]);
			element.addChild(SL("Col1"), val[1]);
		}

		template<>
		void read(const Element& element, Matrix2x2<uint8_t>* out)
		{
			readMatrix2<uint8_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<uint8_t>& val)
		{
			writeMatrix2<uint8_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix2x2<uint16_t>* out)
		{
			readMatrix2<uint16_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<uint16_t>& val)
		{
			writeMatrix2<uint16_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix2x2<uint32_t>* out)
		{
			readMatrix2<uint32_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<uint32_t>& val)
		{
			writeMatrix2<uint32_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix2x2<uint64_t>* out)
		{
			readMatrix2<uint64_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<uint64_t>& val)
		{
			writeMatrix2<uint64_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix2x2<int8_t>* out)
		{
			readMatrix2<int8_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<int8_t>& val)
		{
			writeMatrix2<int8_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix2x2<int16_t>* out)
		{
			readMatrix2<int16_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<int16_t>& val)
		{
			writeMatrix2<int16_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix2x2<int32_t>* out)
		{
			readMatrix2<int32_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<int32_t>& val)
		{
			writeMatrix2<int32_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix2x2<int64_t>* out)
		{
			readMatrix2<int64_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<int64_t>& val)
		{
			writeMatrix2<int64_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix2x2<float32_t>* out)
		{
			readMatrix2<float32_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<float32_t>& val)
		{
			writeMatrix2<float32_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix2x2<float64_t>* out)
		{
			readMatrix2<float64_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix2x2<float64_t>& val)
		{
			writeMatrix2<float64_t>(element, val);
		}

		//////////////////////////////////////

		template<Arithmetic T>
		void readMatrix3(const Element& element, Matrix3x3<T>* out)
		{
			Matrix3x3<T>& out_ref = access_as<Matrix3x3<T>&>(out);

			element.getChild(SL("Col0"), &out_ref[0]);
			element.getChild(SL("Col1"), &out_ref[1]);
			element.getChild(SL("Col2"), &out_ref[2]);
		}

		template<Arithmetic T>
		void writeMatrix3(Element& element, const Matrix3x3<T>& val)
		{
			element.addChild(SL("Col0"), val[0]);
			element.addChild(SL("Col1"), val[1]);
			element.addChild(SL("Col2"), val[2]);
		}

		template<>
		void read(const Element& element, Matrix3x3<uint8_t>* out)
		{
			readMatrix3<uint8_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<uint8_t>& val)
		{
			writeMatrix3<uint8_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix3x3<uint16_t>* out)
		{
			readMatrix3<uint16_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<uint16_t>& val)
		{
			writeMatrix3<uint16_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix3x3<uint32_t>* out)
		{
			readMatrix3<uint32_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<uint32_t>& val)
		{
			writeMatrix3<uint32_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix3x3<uint64_t>* out)
		{
			readMatrix3<uint64_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<uint64_t>& val)
		{
			writeMatrix3<uint64_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix3x3<int8_t>* out)
		{
			readMatrix3<int8_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<int8_t>& val)
		{
			writeMatrix3<int8_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix3x3<int16_t>* out)
		{
			readMatrix3<int16_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<int16_t>& val)
		{
			writeMatrix3<int16_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix3x3<int32_t>* out)
		{
			readMatrix3<int32_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<int32_t>& val)
		{
			writeMatrix3<int32_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix3x3<int64_t>* out)
		{
			readMatrix3<int64_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<int64_t>& val)
		{
			writeMatrix3<int64_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix3x3<float32_t>* out)
		{
			readMatrix3<float32_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<float32_t>& val)
		{
			writeMatrix3<float32_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix3x3<float64_t>* out)
		{
			readMatrix3<float64_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix3x3<float64_t>& val)
		{
			writeMatrix3<float64_t>(element, val);
		}

		//////////////////////////////////////

		template<Arithmetic T>
		void readMatrix4(const Element& element, Matrix4x4<T>* out)
		{
			Matrix4x4<T>& out_ref = access_as<Matrix4x4<T>&>(out);

			element.getChild(SL("Col0"), &out_ref[0]);
			element.getChild(SL("Col1"), &out_ref[1]);
			element.getChild(SL("Col2"), &out_ref[2]);
			element.getChild(SL("Col3"), &out_ref[3]);
		}

		template<Arithmetic T>
		void writeMatrix4(Element& element, const Matrix4x4<T>& val)
		{
			element.addChild(SL("Col0"), val[0]);
			element.addChild(SL("Col1"), val[1]);
			element.addChild(SL("Col2"), val[2]);
			element.addChild(SL("Col3"), val[3]);
		}

		template<>
		void read(const Element& element, Matrix4x4<uint8_t>* out)
		{
			readMatrix4<uint8_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<uint8_t>& val)
		{
			writeMatrix4<uint8_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix4x4<uint16_t>* out)
		{
			readMatrix4<uint16_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<uint16_t>& val)
		{
			writeMatrix4<uint16_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix4x4<uint32_t>* out)
		{
			readMatrix4<uint32_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<uint32_t>& val)
		{
			writeMatrix4<uint32_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix4x4<uint64_t>* out)
		{
			readMatrix4<uint64_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<uint64_t>& val)
		{
			writeMatrix4<uint64_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix4x4<int8_t>* out)
		{
			readMatrix4<int8_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<int8_t>& val)
		{
			writeMatrix4<int8_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix4x4<int16_t>* out)
		{
			readMatrix4<int16_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<int16_t>& val)
		{
			writeMatrix4<int16_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix4x4<int32_t>* out)
		{
			readMatrix4<int32_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<int32_t>& val)
		{
			writeMatrix4<int32_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix4x4<int64_t>* out)
		{
			readMatrix4<int64_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<int64_t>& val)
		{
			writeMatrix4<int64_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix4x4<float32_t>* out)
		{
			readMatrix4<float32_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<float32_t>& val)
		{
			writeMatrix4<float32_t>(element, val);
		}

		template<>
		void read(const Element& element, Matrix4x4<float64_t>* out)
		{
			readMatrix4<float64_t>(element, out);
		}

		template<>
		void write(Element& element, const Matrix4x4<float64_t>& val)
		{
			writeMatrix4<float64_t>(element, val);
		}
	}
}