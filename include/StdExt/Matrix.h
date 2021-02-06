#ifndef _STD_EXT_MATRIX_H_
#define _STD_EXT_MATRIX_H_

#include "Vec.h"

#include "Utility.h"

namespace StdExt
{
	template<VecType vec_t>
	class Matrix2x2
	{
	private:
		Vec2<vec_t> mCols[2];

	public:

		/**
		 * @brief
		 *  Default constructor creates an identity matrix.
		 */
		constexpr Matrix2x2()
		{
			mCols[0] = Vec2<vec_t>(1, 0);
			mCols[1] = Vec2<vec_t>(0, 1);
		}

		constexpr Matrix2x2(const Matrix2x2&) = default;

		constexpr Matrix2x2( vec_t r0c0, vec_t r0c1,
		                     vec_t r1c0, vec_t r1c1 )
		{
			mCols[0] = Vec2<vec_t>(r0c0, r1c0);
			mCols[1] = Vec2<vec_t>(r0c1, r1c1);
		}

		constexpr Matrix2x2(const Vec2<vec_t>& c0, const Vec2<vec_t>& c1) noexcept
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
		Vec2<vec_t> operator*(const Vec2<vec_t>& vec) const noexcept
		{
			return (mCols[0] * vec[0]) +
			       (mCols[1] * vec[1]);
		}

		/**
		 * @brief
		 *  Multiplication of each element of the matrix by a value.
		 */
		Matrix2x2 operator*(vec_t val) const noexcept
		{
			return Matrix2x2( mCols[0] * val,
			                  mCols[1] * val );
		}
		
		/**
		 * @brief
		 *  Division of each element of the matrix by a value.
		 */
		Matrix2x2 operator/(vec_t val) const
		{
			vec_t invVal = vec_t(1.0) / val;

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
		Vec2<vec_t>& operator[](uint16_t index)
		{
			return mCols[index];
		}

		/**
		 * @brief
		 *  Index based access to columns of the matrix.
		 */
		const Vec2<vec_t>& operator[](uint16_t index) const
		{
			return mCols[index];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		vec_t& operator()(uint16_t row, uint16_t column)
		{
			return mCols[column][row];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		const vec_t& operator()(uint16_t row, uint16_t column) const
		{
			return mCols[column][row];
		}

		Matrix2x2 transpose() const noexcept
		{
			return Matrix2x2( mCols[0][0], mCols[0][1],
			                  mCols[1][0], mCols[1][1] );
		}

		vec_t determinant() const noexcept
		{
			return differenceOfProducts<vec_t>(mCols[0][0], mCols[1][1], mCols[1][0], mCols[0][1]);
		}

		Matrix2x2 inverse() const
		{
			
			return Matrix2x2(  mCols[1][1],  -mCols[1][0],
			                  -mCols[0][1],   mCols[0][0] )
				/ determinant();
		}
	};
	
	template<VecType vec_t>
	class Matrix3x3
	{
	private:
		Vec3<vec_t> mCols[3];

		inline vec_t& rc(uint16_t row, uint16_t col)
		{
			return mCols[col][row];
		}

		inline const vec_t& rc(uint16_t row, uint16_t col) const
		{
			return mCols[col][row];
		}

		inline static vec_t dop(vec_t a, vec_t b, vec_t c, vec_t d)
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

		constexpr Matrix3x3( vec_t r0c0, vec_t r0c1, vec_t r0c2,
		                     vec_t r1c0, vec_t r1c1, vec_t r1c2,
		                     vec_t r2c0, vec_t r2c1, vec_t r2c2 )
		{
			mCols[0] = Vec3<vec_t>(r0c0, r1c0, r2c0);
			mCols[1] = Vec3<vec_t>(r0c1, r1c1, r2c1);
			mCols[2] = Vec3<vec_t>(r0c2, r1c2, r2c2);
		}

		constexpr Matrix3x3( const Vec3<vec_t>& c0, const Vec3<vec_t>& c1, const Vec3<vec_t>& c2 ) noexcept
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
		Vec3<vec_t> operator*(const Vec3<vec_t>& vec) const noexcept
		{
			return (mCols[0] * vec[0]) +
			       (mCols[1] * vec[1]) +
				  (mCols[2] * vec[2]);
		}

		/**
		 * @brief
		 *  Multiplication of each element of the matrix by a value.
		 */
		Matrix3x3 operator*(vec_t val) const noexcept
		{
			return Matrix3x3( mCols[0] * val,
			                  mCols[1] * val,
			                  mCols[2] * val );
		}
		
		/**
		 * @brief
		 *  Division of each element of the matrix by a value.
		 */
		Matrix3x3 operator/(vec_t val) const
		{
			vec_t invVal = (vec_t)1.0 / val;

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
		Vec3<vec_t>& operator[](uint16_t index)
		{
			return mCols[index];
		}

		/**
		 * @brief
		 *  Index based access to columns of the matrix.
		 */
		const Vec3<vec_t>& operator[](uint16_t index) const
		{
			return mCols[index];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		vec_t& operator()(uint16_t row, uint16_t column)
		{
			return mCols[column][row];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		const vec_t& operator()(uint16_t row, uint16_t column) const
		{
			return mCols[column][row];
		}

		Matrix3x3 transpose() const noexcept
		{
			return Matrix3x3( mCols[0][0], mCols[0][1], mCols[0][2],
			                  mCols[1][0], mCols[1][1], mCols[1][2],
			                  mCols[2][0], mCols[2][1], mCols[2][2] );
		}

		vec_t determinant() const noexcept
		{
			return   rc(0,0) * dop( rc(1,1), rc(2,2), rc(2,1), rc(1,2) )
			       - rc(0,1) * dop( rc(1,0), rc(2,2), rc(1,2), rc(2,0) )
			       + rc(0,2) * dop( rc(1,0), rc(2,1), rc(1,1), rc(2,0) );
		}

		Matrix3x3 inverse() const
		{
			double invdet = vec_t(1.0) / determinant();

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
	};

	template<VecType vec_t>
	class Matrix4x4
	{
	private:
		Vec4<vec_t> mCols[4];

		inline vec_t& rc(uint16_t row, uint16_t col)
		{
			return mCols[col][row];
		}

		inline const vec_t& rc(uint16_t row, uint16_t col) const
		{
			return mCols[col][row];
		}

		inline static vec_t dop(vec_t a, vec_t b, vec_t c, vec_t d)
		{
			return differenceOfProducts(a, b, c, d);
		}

	public:

		static constexpr Matrix4x4 Identity() noexcept
		{	
			Matrix4x4 ret;

			ret.mCols[0] = Vec4<vec_t>(1, 0, 0, 0);
			ret.mCols[1] = Vec4<vec_t>(0, 1, 0, 0);
			ret.mCols[2] = Vec4<vec_t>(0, 0, 1, 0);
			ret.mCols[2] = Vec4<vec_t>(0, 0, 0, 1);

			return ret;
		}

		constexpr Matrix4x4() = default;
		constexpr Matrix4x4(const Matrix4x4&) = default;

		constexpr Matrix4x4( vec_t r0c0, vec_t r0c1, vec_t r0c2, vec_t r0c3,
		                     vec_t r1c0, vec_t r1c1, vec_t r1c2, vec_t r1c3,
		                     vec_t r2c0, vec_t r2c1, vec_t r2c2, vec_t r2c3,
		                     vec_t r3c0, vec_t r3c1, vec_t r3c2, vec_t r3c3 )
		{
			mCols[0] = Vec4<vec_t>(r0c0, r1c0, r2c0, r3c0);
			mCols[1] = Vec4<vec_t>(r0c1, r1c1, r2c1, r3c1);
			mCols[2] = Vec4<vec_t>(r0c2, r1c2, r2c2, r3c2);
			mCols[3] = Vec4<vec_t>(r0c3, r1c3, r2c3, r3c3);
		}

		constexpr Matrix4x4( const Vec4<vec_t>& c0, const Vec4<vec_t>& c1,
		                     const Vec4<vec_t>& c2, const Vec4<vec_t>& c3 ) noexcept
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
		Vec4<vec_t> operator*(const Vec4<vec_t>& vec) const noexcept
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
		Matrix4x4 operator*(vec_t val) const noexcept
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
		Matrix4x4 operator/(vec_t val) const
		{
			vec_t invVal = (vec_t)1.0 / val;

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
		Vec4<vec_t>& operator[](uint16_t index)
		{
			return mCols[index];
		}

		/**
		 * @brief
		 *  Index based access to columns of the matrix.
		 */
		const Vec4<vec_t>& operator[](uint16_t index) const
		{
			return mCols[index];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		vec_t& operator()(uint16_t row, uint16_t column)
		{
			return mCols[column][row];
		}

		/*
		 * @brief
		 *  Row/Column based access to the elements of the matrix.
		 */
		const vec_t& operator()(uint16_t row, uint16_t column) const
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
			Vec4<vec_t> C1C2;

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

		vec_t determinant() const noexcept
		{
			Vec4<vec_t> Col0;
			Vec4<vec_t> C1C2;

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
			
			constexpr Vec4<vec_t> pnpn(1, -1, 1, -1);
			Col0 *= pnpn;

			Vec4<vec_t> Temp = mCols[0] * Col0;
			return Temp[0] + Temp[1] + Temp[2] + Temp[3];
		}

		Matrix4x4 inverse() const
		{
			constexpr Vec4<vec_t> pnpn(1, -1, 1, -1);
			constexpr Vec4<vec_t> npnp(-1, 1, -1, 1);

			Matrix4x4 C = minors();

			C.mCols[0] *= pnpn;
			C.mCols[1] *= npnp;
			C.mCols[2] *= pnpn;
			C.mCols[3] *= npnp;

			Vec4<vec_t> Temp = mCols[0] * C.mCols[0];

			vec_t Det = Temp[0] + Temp[1] + Temp[2] + Temp[3];

			return C.transpose() * (vec_t(1.0) / Det);
		}
	};
}

#endif // !_STD_EXT_MATRIX_H_