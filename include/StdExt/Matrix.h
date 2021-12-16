#ifndef _STD_EXT_MATRIX_H_
#define _STD_EXT_MATRIX_H_

#include "Vec.h"

#include "Utility.h"
#include "Compare.h"

namespace StdExt
{
	template<Arithmetic num_t>
	class Matrix2x2
	{
	private:
		Vec2<num_t> mCols[2];

	public:

		/**
		 * @brief
		 *  Default constructor creates an identity matrix.
		 */
		constexpr Matrix2x2()
		{
			mCols[0] = Vec2<num_t>(1, 0);
			mCols[1] = Vec2<num_t>(0, 1);
		}

		constexpr Matrix2x2(const Matrix2x2&) = default;

		constexpr Matrix2x2( num_t r0c0, num_t r0c1,
		                     num_t r1c0, num_t r1c1 )
		{
			mCols[0] = Vec2<num_t>(r0c0, r1c0);
			mCols[1] = Vec2<num_t>(r0c1, r1c1);
		}

		constexpr Matrix2x2(const Vec2<num_t>& c0, const Vec2<num_t>& c1) noexcept
		{
			mCols[0] = c0;
			mCols[1] = c1;
		}

		/**
		 * @brief
		 *  (This matrix) x right.
		 */
		Matrix2x2 operator*(const Matrix2x2& right) const noexcept
		{
			return Matrix2x2( (*this) * right.mCols[0],
			                  (*this) * right.mCols[1] );
		}

		/**
		 * @brief
		 *  Multiplication of the matrix and a column vector.
		 */
		Vec2<num_t> operator*(const Vec2<num_t>& vec) const noexcept
		{
			return (mCols[0] * vec[0]) +
			       (mCols[1] * vec[1]);
		}

		/**
		 * @brief
		 *  Multiplication of each element of the matrix by a value.
		 */
		Matrix2x2 operator*(num_t val) const noexcept
		{
			return Matrix2x2( mCols[0] * val,
			                  mCols[1] * val );
		}
		
		/**
		 * @brief
		 *  Division of each element of the matrix by a value.
		 */
		Matrix2x2 operator/(num_t val) const
		{
			num_t invVal = num_t(1.0) / val;

			return Matrix2x2( mCols[0] * invVal,
			                  mCols[1] * invVal );
		}

		/**
		 * @brief
		 *  Componentwise addition of the elements of the matrix.
		 */
		Matrix2x2 operator+(const Matrix2x2& mat) const
		{
			return Matrix2x2( mCols[0] + mat.mCols[0],
			                  mCols[1] + mat.mCols[1] );
		}

		/**
		 * @brief
		 *  Componentwise addition of the elements of the matrix.
		 */
		Matrix2x2 operator-(const Matrix2x2& mat) const
		{
			return Matrix2x2( mCols[0] - mat.mCols[0],
			                  mCols[1] - mat.mCols[1] );
		}

		/**
		 * @brief
		 *  Index based access to columns of the matrix.
		 */
		Vec2<num_t>& operator[](uint16_t index)
		{
			return mCols[index];
		}

		/**
		 * @brief
		 *  Index based access to columns of the matrix.
		 */
		const Vec2<num_t>& operator[](uint16_t index) const
		{
			return mCols[index];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		num_t& operator()(uint16_t row, uint16_t column)
		{
			return mCols[column][row];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		const num_t& operator()(uint16_t row, uint16_t column) const
		{
			return mCols[column][row];
		}

		Matrix2x2 transpose() const noexcept
		{
			return Matrix2x2( mCols[0][0], mCols[0][1],
			                  mCols[1][0], mCols[1][1] );
		}

		num_t determinant() const noexcept
		{
			return differenceOfProducts<num_t>(mCols[0][0], mCols[1][1], mCols[1][0], mCols[0][1]);
		}

		Matrix2x2 inverse() const
		{
			
			return Matrix2x2(  mCols[1][1],  -mCols[1][0],
			                  -mCols[0][1],   mCols[0][0] )
				/ determinant();
		}

		int compare(const Matrix2x2<num_t>& other) const
		{
			return StdExt::compare(
				mCols[0], other.mCols[0],
				mCols[1], other.mCols[1]
			);
		}
	};
	
	template<Arithmetic num_t>
	class Matrix3x3
	{
	private:
		Vec3<num_t> mCols[3];

		inline num_t& rc(uint16_t row, uint16_t col)
		{
			return mCols[col][row];
		}

		inline const num_t& rc(uint16_t row, uint16_t col) const
		{
			return mCols[col][row];
		}

		inline static num_t dop(num_t a, num_t b, num_t c, num_t d)
		{
			return differenceOfProducts(a, b, c, d);
		}

	public:
		static constexpr Matrix3x3 identity()
		{
			return Matrix3x3( 1, 0, 0,
			                  0, 1, 0,
			                  0, 0, 1 );
		}

		constexpr Matrix3x3() = default;
		constexpr Matrix3x3(const Matrix3x3&) = default;

		constexpr Matrix3x3( num_t r0c0, num_t r0c1, num_t r0c2,
		                     num_t r1c0, num_t r1c1, num_t r1c2,
		                     num_t r2c0, num_t r2c1, num_t r2c2 )
		{
			mCols[0] = Vec3<num_t>(r0c0, r1c0, r2c0);
			mCols[1] = Vec3<num_t>(r0c1, r1c1, r2c1);
			mCols[2] = Vec3<num_t>(r0c2, r1c2, r2c2);
		}

		constexpr Matrix3x3( const Vec3<num_t>& c0, const Vec3<num_t>& c1, const Vec3<num_t>& c2 ) noexcept
		{
			mCols[0] = c0;
			mCols[1] = c1;
			mCols[2] = c2;
		}

		/**
		 * @brief
		 *  (This matrix) x right.
		 */
		Matrix3x3 operator*(const Matrix3x3& right) const noexcept
		{
			return Matrix3x3( (*this) * right.mCols[0],
			                  (*this) * right.mCols[1],
			                  (*this) * right.mCols[2]);
		}

		/**
		 * @brief
		 *  Multiplication of the matrix and a column vector.
		 */
		Vec3<num_t> operator*(const Vec3<num_t>& vec) const noexcept
		{
			return (mCols[0] * vec[0]) +
			       (mCols[1] * vec[1]) +
				  (mCols[2] * vec[2]);
		}

		/**
		 * @brief
		 *  Multiplication of each element of the matrix by a value.
		 */
		Matrix3x3 operator*(num_t val) const noexcept
		{
			return Matrix3x3( mCols[0] * val,
			                  mCols[1] * val,
			                  mCols[2] * val );
		}
		
		/**
		 * @brief
		 *  Division of each element of the matrix by a value.
		 */
		Matrix3x3 operator/(num_t val) const
		{
			num_t invVal = (num_t)1.0 / val;

			return Matrix3x3( mCols[0] * invVal,
			                  mCols[1] * invVal,
			                  mCols[2] * invVal );
		}

		/**
		 * @brief
		 *  Componentwise addition of the elements of the matrix.
		 */
		Matrix3x3 operator+(const Matrix3x3& mat) const
		{
			return Matrix3x3( mCols[0] + mat.mCols[0],
			                  mCols[1] + mat.mCols[1],
			                  mCols[2] + mat.mCols[2] );
		}

		/**
		 * @brief
		 *  Componentwise addition of the elements of the matrix.
		 */
		Matrix3x3 operator-(const Matrix3x3& mat) const
		{
			return Matrix3x3( mCols[0] - mat.mCols[0],
			                  mCols[1] - mat.mCols[1],
			                  mCols[2] - mat.mCols[2] );
		}

		/**
		 * @brief
		 *  Index based access to columns of the matrix.
		 */
		Vec3<num_t>& operator[](uint16_t index)
		{
			return mCols[index];
		}

		/**
		 * @brief
		 *  Index based access to columns of the matrix.
		 */
		const Vec3<num_t>& operator[](uint16_t index) const
		{
			return mCols[index];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		num_t& operator()(uint16_t row, uint16_t column)
		{
			return mCols[column][row];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		const num_t& operator()(uint16_t row, uint16_t column) const
		{
			return mCols[column][row];
		}

		Matrix3x3 transpose() const noexcept
		{
			return Matrix3x3( mCols[0][0], mCols[0][1], mCols[0][2],
			                  mCols[1][0], mCols[1][1], mCols[1][2],
			                  mCols[2][0], mCols[2][1], mCols[2][2] );
		}

		num_t determinant() const noexcept
		{
			return   rc(0,0) * dop( rc(1,1), rc(2,2), rc(2,1), rc(1,2) )
			       - rc(0,1) * dop( rc(1,0), rc(2,2), rc(1,2), rc(2,0) )
			       + rc(0,2) * dop( rc(1,0), rc(2,1), rc(1,1), rc(2,0) );
		}

		Matrix3x3 inverse() const
		{
			double invdet = num_t(1.0) / determinant();

			Matrix3x3 ret;

			ret(0,0) =  dop(rc(1,1), rc(2,2), rc(2,1), rc(1,2)) * invdet;
			ret(0,1) = -dop(rc(0,1), rc(2,2), rc(0,2), rc(2,1)) * invdet;
			ret(0,2) =  dop(rc(0,1), rc(1,2), rc(0,2), rc(1,1)) * invdet;
			ret(1,0) = -dop(rc(1,0), rc(2,2), rc(1,2), rc(2,0)) * invdet;
			ret(1,1) =  dop(rc(0,0), rc(2,2), rc(0,2), rc(2,0)) * invdet;
			ret(1,2) = -dop(rc(0,0), rc(1,2), rc(1,0), rc(0,2)) * invdet;
			ret(2,0) =  dop(rc(1,0), rc(2,1), rc(2,0), rc(1,1)) * invdet;
			ret(2,1) = -dop(rc(0,0), rc(2,1), rc(2,0), rc(0,1)) * invdet;
			ret(2,2) =  dop(rc(0,0), rc(1,1), rc(1,0), rc(0,1)) * invdet;

			return ret;
		}

		int compare(const Matrix3x3<num_t>& other) const
		{
			return StdExt::compare(
				mCols[0], other.mCols[0],
				mCols[1], other.mCols[1],
				mCols[2], other.mCols[2]
			);
		}
	};

	template<Arithmetic num_t>
	class Matrix4x4
	{
	private:
		Vec4<num_t> mCols[4];

		inline num_t& rc(uint16_t row, uint16_t col)
		{
			return mCols[col][row];
		}

		inline const num_t& rc(uint16_t row, uint16_t col) const
		{
			return mCols[col][row];
		}

		inline static num_t dop(num_t a, num_t b, num_t c, num_t d)
		{
			return differenceOfProducts(a, b, c, d);
		}

	public:

		static constexpr Matrix4x4 Identity() noexcept
		{	
			Matrix4x4 ret;

			ret.mCols[0] = Vec4<num_t>(1, 0, 0, 0);
			ret.mCols[1] = Vec4<num_t>(0, 1, 0, 0);
			ret.mCols[2] = Vec4<num_t>(0, 0, 1, 0);
			ret.mCols[2] = Vec4<num_t>(0, 0, 0, 1);

			return ret;
		}

		constexpr Matrix4x4() = default;
		constexpr Matrix4x4(const Matrix4x4&) = default;

		constexpr Matrix4x4( num_t r0c0, num_t r0c1, num_t r0c2, num_t r0c3,
		                     num_t r1c0, num_t r1c1, num_t r1c2, num_t r1c3,
		                     num_t r2c0, num_t r2c1, num_t r2c2, num_t r2c3,
		                     num_t r3c0, num_t r3c1, num_t r3c2, num_t r3c3 )
		{
			mCols[0] = Vec4<num_t>(r0c0, r1c0, r2c0, r3c0);
			mCols[1] = Vec4<num_t>(r0c1, r1c1, r2c1, r3c1);
			mCols[2] = Vec4<num_t>(r0c2, r1c2, r2c2, r3c2);
			mCols[3] = Vec4<num_t>(r0c3, r1c3, r2c3, r3c3);
		}

		constexpr Matrix4x4( const Vec4<num_t>& c0, const Vec4<num_t>& c1,
		                     const Vec4<num_t>& c2, const Vec4<num_t>& c3 ) noexcept
		{
			mCols[0] = c0;
			mCols[1] = c1;
			mCols[2] = c2;
			mCols[3] = c3;
		}

		/**
		 * @brief
		 *  (This matrix) x right.
		 */
		Matrix4x4 operator*(const Matrix4x4& right) const noexcept
		{
			return Matrix4x4( (*this) * right.mCols[0],
			                  (*this) * right.mCols[1],
			                  (*this) * right.mCols[2],
			                  (*this) * right.mCols[3] );
		}

		/**
		 * @brief
		 *  Multiplication of the matrix and a column vector.
		 */
		Vec4<num_t> operator*(const Vec4<num_t>& vec) const noexcept
		{
			return (mCols[0] * vec[0]) +
			       (mCols[1] * vec[1]) +
			       (mCols[2] * vec[2]) +
			       (mCols[3] * vec[3]);
		}

		/**
		 * @brief
		 *  Multiplication of each element of the matrix by a value.
		 */
		Matrix4x4 operator*(num_t val) const noexcept
		{
			return Matrix4x4( mCols[0] * val,
			                  mCols[1] * val,
			                  mCols[2] * val,
			                  mCols[3] * val );
		}
		
		/**
		 * @brief
		 *  Division of each element of the matrix by a value.
		 */
		Matrix4x4 operator/(num_t val) const
		{
			num_t invVal = (num_t)1.0 / val;

			return Matrix4x4( mCols[0] * invVal,
			                  mCols[1] * invVal,
			                  mCols[2] * invVal,
			                  mCols[3] * invVal );
		}

		/**
		 * @brief
		 *  Componentwise addition of the elements of the matrix.
		 */
		Matrix4x4 operator+(const Matrix4x4& mat) const
		{
			return Matrix4x4( mCols[0] + mat.mCols[0],
			                  mCols[1] + mat.mCols[1],
			                  mCols[2] + mat.mCols[2],
			                  mCols[3] + mat.mCols[3] );
		}

		/**
		 * @brief
		 *  Componentwise addition of the elements of the matrix.
		 */
		Matrix4x4 operator-(const Matrix4x4& mat) const
		{
			return Matrix4x4( mCols[0] - mat.mCols[0],
			                  mCols[1] - mat.mCols[1],
			                  mCols[2] - mat.mCols[2],
			                  mCols[3] - mat.mCols[3] );
		}

		/**
		 * @brief
		 *  Index based access to columns of the matrix.
		 */
		Vec4<num_t>& operator[](uint16_t index)
		{
			return mCols[index];
		}

		/**
		 * @brief
		 *  Index based access to columns of the matrix.
		 */
		const Vec4<num_t>& operator[](uint16_t index) const
		{
			return mCols[index];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		num_t& operator()(uint16_t row, uint16_t column)
		{
			return mCols[column][row];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		const num_t& operator()(uint16_t row, uint16_t column) const
		{
			return mCols[column][row];
		}

		Matrix4x4 transpose() const noexcept
		{
			Matrix4x4 Tmp;
			Matrix4x4 Ret;

			Tmp.mCols[0] = shuffle<0,1,0,1>(mCols[0], mCols[1]);
			Tmp.mCols[2] = shuffle<2,3,2,3>(mCols[0], mCols[1]);
			Tmp.mCols[1] = shuffle<0,1,0,1>(mCols[2], mCols[3]);
			Tmp.mCols[3] = shuffle<2,3,2,3>(mCols[2], mCols[3]);

			Ret.mCols[0] = shuffle<0,2,0,2>(Tmp.mCols[0], Tmp.mCols[1]);
			Ret.mCols[1] = shuffle<1,3,1,3>(Tmp.mCols[0], Tmp.mCols[1]);
			Ret.mCols[2] = shuffle<0,2,0,2>(Tmp.mCols[2], Tmp.mCols[3]);
			Ret.mCols[3] = shuffle<1,3,1,3>(Tmp.mCols[2], Tmp.mCols[3]);

			return Ret;
		}

		Matrix4x4 minors() const noexcept
		{
			Matrix4x4 Ret;
			Vec4<num_t> C1C2;

			// The first column
			C1C2 = mCols[1] * shuffle<1,2,3,0>(mCols[2], mCols[2]);

			Ret.mCols[0] = shuffle<1,2,3,0>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[3], mCols[3]);

			Ret.mCols[0] += shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[3], mCols[3]);

			Ret.mCols[0] += shuffle<3,0,1,2>(mCols[1], mCols[1])
				* shuffle<1,2,3,0>(mCols[2], mCols[2])
				* shuffle<2,3,0,1>(mCols[3], mCols[3]);

			C1C2 = mCols[1] * shuffle<3,0,1,2>(mCols[2], mCols[2]);

			Ret.mCols[0] -= shuffle<3,0,1,2>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[3], mCols[3]);

			Ret.mCols[0] -= shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[3], mCols[3]);

			Ret.mCols[0] -= shuffle<1,2,3,0>(mCols[1], mCols[1])
				* shuffle<3,0,1,2>(mCols[2], mCols[2])
				* shuffle<2,3,0,1>(mCols[3], mCols[3]);


			// The Second Column
			C1C2 = mCols[0] * shuffle<1,2,3,0>(mCols[2], mCols[2]);

			Ret.mCols[1] = shuffle<1,2,3,0>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[3], mCols[3]);

			Ret.mCols[1] += shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[3], mCols[3]);

			Ret.mCols[1] += shuffle<3,0,1,2>(mCols[0], mCols[0])
				* shuffle<1,2,3,0>(mCols[2], mCols[2])
				* shuffle<2,3,0,1>(mCols[3], mCols[3]);

			C1C2 = mCols[0] * shuffle<3,0,1,2>(mCols[2], mCols[2]);

			Ret.mCols[1] -= shuffle<3,0,1,2>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[3], mCols[3]);

			Ret.mCols[1] -= shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[3], mCols[3]);

			Ret.mCols[1] -= shuffle<1,2,3,0>(mCols[0], mCols[0])
				* shuffle<3,0,1,2>(mCols[2], mCols[2])
				* shuffle<2,3,0,1>(mCols[3], mCols[3]);


			// The Third Column
			C1C2 = mCols[0] * shuffle<1,2,3,0>(mCols[1], mCols[1]);
			
			Ret.mCols[2] = shuffle<1,2,3,0>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[3], mCols[3]);

			Ret.mCols[2] += shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[3], mCols[3]);

			Ret.mCols[2] += shuffle<3,0,1,2>(mCols[0], mCols[0])
				* shuffle<1,2,3,0>(mCols[1], mCols[1])
				* shuffle<2,3,0,1>(mCols[3], mCols[3]);

			C1C2 = mCols[0] * shuffle<3,0,1,2>(mCols[1], mCols[1]);

			Ret.mCols[2] -= shuffle<3,0,1,2>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[3], mCols[3]);

			Ret.mCols[2] -= shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[3], mCols[3]);

			Ret.mCols[2] -= shuffle<1,2,3,0>(mCols[0], mCols[0])
				* shuffle<3,0,1,2>(mCols[1], mCols[1])
				* shuffle<2,3,0,1>(mCols[3], mCols[3]);

			// The Fourth Column
			C1C2 = mCols[0] * shuffle<1,2,3,0>(mCols[1], mCols[1]);

			Ret.mCols[3] = shuffle<1,2,3,0>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[2], mCols[2]);

			Ret.mCols[3] += shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[2], mCols[2]);

			Ret.mCols[3] += shuffle<3,0,1,2>(mCols[0], mCols[0])
				* shuffle<1,2,3,0>(mCols[1], mCols[1])
				* shuffle<2,3,0,1>(mCols[2], mCols[2]);

			C1C2 = mCols[0] * shuffle<3,0,1,2>(mCols[1], mCols[1]);

			Ret.mCols[3] -= shuffle<3,0,1,2>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[2], mCols[2]);

			Ret.mCols[3] -= shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[2], mCols[2]);

			Ret.mCols[3] -= shuffle<1,2,3,0>(mCols[0], mCols[0])
				* shuffle<3,0,1,2>(mCols[1], mCols[1])
				* shuffle<2,3,0,1>(mCols[2], mCols[2]);

			return Ret;
		}

		num_t determinant() const noexcept
		{
			Vec4<num_t> Col0;
			Vec4<num_t> C1C2;

			// The first column
			C1C2 = mCols[1] * shuffle<1,2,3,0>(mCols[2], mCols[2]);

			Col0 = shuffle<1,2,3,0>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[3], mCols[3]);

			Col0 += shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[3], mCols[3]);

			Col0 += shuffle<3,0,1,2>(mCols[1], mCols[1])
				* shuffle<1,2,3,0>(mCols[2], mCols[2])
				* shuffle<2,3,0,1>(mCols[3], mCols[3]);

			C1C2 = mCols[1] * shuffle<3,0,1,2>(mCols[2], mCols[2]);

			Col0 -= shuffle<3,0,1,2>(C1C2, C1C2)
				* shuffle<1,2,3,0>(mCols[3], mCols[3]);

			Col0 -= shuffle<2,3,0,1>(C1C2, C1C2)
				* shuffle<3,0,1,2>(mCols[3], mCols[3]);

			Col0 -= shuffle<1,2,3,0>(mCols[1], mCols[1])
				* shuffle<3,0,1,2>(mCols[2], mCols[2])
				* shuffle<2,3,0,1>(mCols[3], mCols[3]);
			
			constexpr Vec4<num_t> pnpn(1, -1, 1, -1);
			Col0 *= pnpn;

			Vec4<num_t> Temp = mCols[0] * Col0;
			return Temp[0] + Temp[1] + Temp[2] + Temp[3];
		}

		Matrix4x4 inverse() const
		{
			constexpr Vec4<num_t> pnpn(1, -1, 1, -1);
			constexpr Vec4<num_t> npnp(-1, 1, -1, 1);

			Matrix4x4 C = minors();

			C.mCols[0] *= pnpn;
			C.mCols[1] *= npnp;
			C.mCols[2] *= pnpn;
			C.mCols[3] *= npnp;

			Vec4<num_t> Temp = mCols[0] * C.mCols[0];

			num_t Det = Temp[0] + Temp[1] + Temp[2] + Temp[3];

			return C.transpose() * (num_t(1.0) / Det);
		}

		int compare(const Matrix4x4<num_t>& other) const
		{
			return StdExt::compare(
				mCols[0], other.mCols[0],
				mCols[1], other.mCols[1],
				mCols[2], other.mCols[2],
				mCols[3], other.mCols[3]
			);
		}
	};

	namespace Serialize::Binary
	{
		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<uint8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<uint8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<uint16_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<uint32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<uint64_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<int8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<int16_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<int32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<int64_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<float32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix2x2<float64_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix2x2<float64_t>& val);

		//////////////////////////////////////

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<uint8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<uint8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<uint16_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<uint32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<uint64_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<int8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<int16_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<int32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<int64_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<float32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix3x3<float64_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix3x3<float64_t>& val);

		//////////////////////////////////////

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<uint8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<uint8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<uint16_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<uint32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<uint64_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<int8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<int16_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<int32_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<int64_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<float32_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Matrix4x4<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Matrix4x4<float64_t>& val);
	}

	namespace Serialize::XML
	{
		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<uint16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<uint32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<uint64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<int8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<int16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<int32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<int64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<float32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix2x2<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix2x2<float64_t>& val);

		//////////////////////////////////////

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<uint16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<uint32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<uint64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<int8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<int16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<int32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<int64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<float32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix3x3<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix3x3<float64_t>& val);

		//////////////////////////////////////

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<uint16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<uint32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<uint64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<int8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<int16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<int32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<int64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<float32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Matrix4x4<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Matrix4x4<float64_t>& val);
	}
}

#endif // !_STD_EXT_MATRIX_H_