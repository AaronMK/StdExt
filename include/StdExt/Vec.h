#ifndef _ENGINE_VEC_H_
#define _ENGINE_VEC_H_

#include "StdExt.h"

#include "Concepts.h"
#include "Utility.h"
#include "Memory.h"

#ifndef __SSE__
#	define __SSE__
#endif

#ifndef __MMX__
#	define __MMX__
#endif

#include <xmmintrin.h>
#include <assert.h>

namespace StdExt
{
	template<typename T>
	concept VecType = std::same_as<T, bool> || Arithmetic<T>;

	template<VecType vec_t>
	class Vec2
	{
	private:
		vec_t mValues[2];

	public:
		constexpr Vec2() = default;
		constexpr Vec2(const Vec2&) = default;

		constexpr Vec2(vec_t val)
		{
			mValues = {val, val};
		}

		constexpr Vec2(vec_t v0, vec_t v1) noexcept
		{
			mValues[0] = v0;
			mValues[1] = v1;
		}

		vec_t& operator[](uint16_t index) noexcept
		{
			return mValues[index];
		}

		const vec_t& operator[](uint16_t index) const noexcept
		{
			return mValues[index];
		};

		Vec2 operator+(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] + right[0],
				mValues[1] + right[1]
			);
		}

		void operator+=(const Vec2& right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] += right[0];
			mValues[1] += right[1];
		}

		Vec2 operator-(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] - right[0],
				mValues[1] - right[1]
			);
		}

		void  operator-=(const Vec2& right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] -= right[0];
			mValues[1] -= right[1];
		}

		Vec2 operator*(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] * right[0],
				mValues[1] * right[1]
			);
		}

		Vec2 operator*(vec_t right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] * right,
				mValues[1] * right
			);
		}

		void operator*=(const Vec2& right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] *= right[0];
			mValues[1] *= right[1];
		}

		void operator*=(vec_t right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] *= right;
			mValues[1] *= right;
		}

		Vec2 operator/(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] / right[0],
				mValues[1] / right[1]
			);
		}

		Vec2 operator/(vec_t right) const noexcept
			requires Arithmetic<vec_t>
		{
			if constexpr( FloatingPoint<vec_t> )
			{
				return (*this) * ( (vec_t)1.0 / right);
			}
			else
			{
				return Vec2(
					mValues[0] / right,
					mValues[1] / right
				);
			}
		}

		Vec2 operator/=(const Vec2& right) noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] /= right[0],
				mValues[1] /= right[1]
			);
		}

		void operator/=(vec_t right) noexcept
			requires Arithmetic<vec_t>
		{
			if constexpr( FloatingPoint<vec_t> )
			{
				(*this) *= ( (vec_t)1.0 / right);
			}
			else
			{
				mValues[0] /= right;
				mValues[1] /= right;
			}
		}

		Vec2 operator<(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] < right[0],
				mValues[1] < right[1]
			);
		}

		Vec2 operator<=(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] <= right[0],
				mValues[1] <= right[1]
			);
		}

		Vec2 operator==(const Vec2& right) const noexcept
		{
			return Vec2(
				mValues[0] == right[0],
				mValues[1] == right[1]
			);
		}

		Vec2 operator!=(const Vec2& right) const noexcept
		{
			return Vec2(
				mValues[0] != right[0],
				mValues[1] != right[1]
			);
		}
		Vec2 operator>=(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] >= right[0],
				mValues[1] >= right[1]
			);
		}

		Vec2 operator>(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2(
				mValues[0] > right[0],
				mValues[1] > right[1]
			);
		}

		/**
		 * @brief
		 *  The sum of all components in the Vec3.
		 */
		vec_t sum() const
		{
			return mValues[0] + mValues[1];
		}

		/**
		 * @brief
		 *  The maximum value in the Vec2
		 */
		vec_t max()
		{
			return std::max(mValues[0], mValues[1]);
		}

		/**
		 * @brief
		 *  The minimum value in the Vec2
		 */
		vec_t min()
		{
			return std::min(mValues[0], mValues[1]);
		}

		int compare(const Vec2& other) const noexcept
		{
			return StdExt::approxCompare(
				mValues[0], other[0],
				mValues[1], other[1]
			);
		}
	};
	
	template<VecType vec_t>
	class Vec3
	{
	private:
		vec_t mValues[3];

	public:
		constexpr Vec3() = default;
		constexpr Vec3(const Vec3&) = default;

		constexpr Vec3(vec_t val)
		{
			mValues = {val, val, val};
		}

		constexpr Vec3(vec_t v0, vec_t v1, vec_t v2) noexcept
		{
			mValues[0] = v0;
			mValues[1] = v1;
			mValues[2] = v2;
		}

		vec_t& operator[](uint16_t index) noexcept
		{
			return mValues[index];
		}

		const vec_t& operator[](uint16_t index) const noexcept
		{
			return mValues[index];
		};

		Vec3 operator+(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] + right[0],
				mValues[1] + right[1],
				mValues[2] + right[2]
			);
		}

		void operator+=(const Vec3& right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] += right[0];
			mValues[1] += right[1];
			mValues[2] += right[2];
		}

		Vec3 operator-(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] - right[0],
				mValues[1] - right[1],
				mValues[2] - right[2]
			);
		}

		void  operator-=(const Vec3& right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] -= right[0];
			mValues[1] -= right[1];
			mValues[2] -= right[2];
		}

		Vec3 operator*(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] * right[0],
				mValues[1] * right[1],
				mValues[2] * right[2]
			);
		}

		Vec3 operator*(vec_t right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] * right,
				mValues[1] * right,
				mValues[2] * right
			);
		}

		void operator*=(const Vec3& right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] *= right[0];
			mValues[1] *= right[1];
			mValues[2] *= right[2];
		}

		void operator*=(vec_t right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] *= right;
			mValues[1] *= right;
			mValues[2] *= right;
		}

		Vec3 operator/(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] / right[0],
				mValues[1] / right[1],
				mValues[2] / right[2]
			);
		}

		Vec3 operator/(vec_t right) const noexcept
			requires Arithmetic<vec_t>
		{
			if constexpr( FloatingPoint<vec_t> )
			{
				return (*this) * ( (vec_t)1.0 / right);
			}
			else
			{
				return Vec3(
					mValues[0] / right,
					mValues[1] / right,
					mValues[2] / right
				);
			}
		}

		Vec3 operator/=(const Vec3& right) noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] /= right[0],
				mValues[1] /= right[1],
				mValues[2] /= right[2]
			);
		}

		void operator/=(vec_t right) noexcept
			requires Arithmetic<vec_t>
		{
			if constexpr( FloatingPoint<vec_t> )
			{
				(*this) *= ( (vec_t)1.0 / right);
			}
			else
			{
				mValues[0] /= right;
				mValues[1] /= right;
				mValues[2] /= right;
			}
		}

		Vec3 operator<(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] < right[0],
				mValues[1] < right[1],
				mValues[2] < right[2]
			);
		}

		Vec3 operator<=(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] <= right[0],
				mValues[1] <= right[1],
				mValues[2] <= right[2]
			);
		}

		Vec3 operator==(const Vec3& right) const noexcept
		{
			return Vec3(
				mValues[0] == right[0],
				mValues[1] == right[1],
				mValues[2] == right[2]
			);
		}

		Vec3 operator!=(const Vec3& right) const noexcept
		{
			return Vec3(
				mValues[0] != right[0],
				mValues[1] != right[1],
				mValues[2] != right[2]
			);
		}

		Vec3 operator>=(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] >= right[0],
				mValues[1] >= right[1],
				mValues[2] >= right[2]
			);
		}

		Vec3 operator>(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3(
				mValues[0] > right[0],
				mValues[1] > right[1],
				mValues[2] > right[2]
			);
		}

		/**
		 * @brief
		 *  The sum of all components in the Vec3.
		 */
		vec_t sum() const
		{
			return mValues[0] + mValues[1] + mValues[2];
		}

		/**
		 * @brief
		 *  The maximum value in the Vec3
		 */
		vec_t max()
		{
			return std::max(
				std::max(mValues[0], mValues[1]),
				mValues[2]
			);
		}

		/**
		 * @brief
		 *  The minimum value in the Vec3
		 */
		vec_t min()
		{
			return std::min(
				std::min(mValues[0], mValues[1]),
				mValues[2]
			);
		}

		int compare(const Vec3& other) const noexcept
		{
			return StdExt::approxCompare(
				mValues[0], other[0],
				mValues[1], other[1],
				mValues[2], other[2]
			);
		}
	};
	
	template<VecType vec_t>
	class Vec4
	{
	private:
		vec_t mValues[4];

	public:
		constexpr Vec4() = default;
		constexpr Vec4(const Vec4&) = default;

		constexpr Vec4(vec_t val)
		{
			mValues = {val, val, val, val};
		}

		constexpr Vec4(vec_t v0, vec_t v1, vec_t v2, vec_t v3) noexcept
		{
			mValues[0] = v0;
			mValues[1] = v1;
			mValues[2] = v2;
			mValues[3] = v3;
		}

		vec_t& operator[](uint16_t index) noexcept
		{
			return mValues[index];
		}

		const vec_t& operator[](uint16_t index) const noexcept
		{
			return mValues[index];
		};

		Vec4 operator+(const Vec4& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] + right[0],
				mValues[1] + right[1],
				mValues[2] + right[2],
				mValues[3] + right[3]
			);
		}

		void operator+=(const Vec4& right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] += right[0];
			mValues[1] += right[1];
			mValues[2] += right[2];
			mValues[3] += right[3];
		}

		Vec4 operator-(const Vec4& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] - right[0],
				mValues[1] - right[1],
				mValues[2] - right[2],
				mValues[3] - right[3]
			);
		}

		void  operator-=(const Vec4& right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] -= right[0];
			mValues[1] -= right[1];
			mValues[2] -= right[2];
			mValues[3] -= right[3];
		}

		Vec4 operator*(const Vec4& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] * right[0],
				mValues[1] * right[1],
				mValues[2] * right[2],
				mValues[3] * right[3]
			);
		}

		Vec4 operator*(vec_t right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] * right,
				mValues[1] * right,
				mValues[2] * right,
				mValues[3] * right
			);
		}

		void operator*=(const Vec4& right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] *= right[0];
			mValues[1] *= right[1];
			mValues[2] *= right[2];
			mValues[3] *= right[3];
		}

		void operator*=(vec_t right) noexcept
			requires Arithmetic<vec_t>
		{
			mValues[0] *= right;
			mValues[1] *= right;
			mValues[2] *= right;
			mValues[3] *= right;
		}

		Vec4 operator/(const Vec4& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] / right[0],
				mValues[1] / right[1],
				mValues[2] / right[2],
				mValues[3] / right[3]
			);
		}

		Vec4 operator/(vec_t right) const noexcept
			requires Arithmetic<vec_t>
		{
			if constexpr( FloatingPoint<vec_t> )
			{
				return (*this) * ( (vec_t)1.0 / right);
			}
			else
			{
				return Vec4(
					mValues[0] / right,
					mValues[1] / right,
					mValues[2] / right,
					mValues[3] / right
				);
			}
		}

		Vec4 operator/=(const Vec4& right) noexcept
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] /= right[0],
				mValues[1] /= right[1],
				mValues[2] /= right[2],
				mValues[3] /= right[3]
			);
		}

		void operator/=(vec_t right) noexcept
			requires Arithmetic<vec_t>
		{
			if constexpr( FloatingPoint<vec_t> )
			{
				(*this) *= ( (vec_t)1.0 / right);
			}
			else
			{
				mValues[0] /= right;
				mValues[1] /= right;
				mValues[2] /= right;
				mValues[3] /= right;
			}
		}

		Vec4 operator<(const Vec4& right) const
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] < right[0],
				mValues[1] < right[1],
				mValues[2] < right[2],
				mValues[3] < right[3]
			);
		}

		Vec4 operator<=(const Vec4& right) const
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] <= right[0],
				mValues[1] <= right[1],
				mValues[2] <= right[2],
				mValues[3] <= right[3]
			);
		}

		Vec4 operator==(const Vec4& right) const
		{
			return Vec4(
				mValues[0] == right[0],
				mValues[1] == right[1],
				mValues[2] == right[2],
				mValues[3] == right[3]
			);
		}

		Vec4 operator!=(const Vec4& right) const
		{
			return Vec4(
				mValues[0] != right[0],
				mValues[1] != right[1],
				mValues[2] != right[2],
				mValues[3] != right[3]
			);
		}

		Vec4 operator>=(const Vec4& right) const
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] >= right[0],
				mValues[1] >= right[1],
				mValues[2] >= right[2],
				mValues[3] >= right[3]
			);
		}

		Vec4 operator>(const Vec4& right) const
			requires Arithmetic<vec_t>
		{
			return Vec4(
				mValues[0] > right[0],
				mValues[1] > right[1],
				mValues[2] > right[2],
				mValues[3] > right[3]
			);
		}

		/**
		 * @brief
		 *  The sum of all components in the Vec4.
		 */
		vec_t sum() const
		{
			return mValues[0] + mValues[1] + mValues[2] + mValues[3];
		}

		/**
		 * @brief
		 *  The maximum value in the Vec4
		 */
		vec_t max()
		{
			return std::max(
				std::max(mValues[0], mValues[1]),
				std::max(mValues[2], mValues[3])
			);
		}

		/**
		 * @brief
		 *  The minimum value in the Vec4
		 */
		vec_t min()
		{
			return std::min(
				std::min(mValues[0], mValues[1]),
				std::min(mValues[2], mValues[3])
			);
		}

		int compare(const Vec4& other) const noexcept
		{
			return StdExt::approxCompare(
				mValues[0], other[0],
				mValues[1], other[1],
				mValues[2], other[2],
				mValues[3], other[3]
			);
		}
	};

	template<>
	class Vec4<float32_t>
	{
	private:
		__m128 mValues;

	public:
		Vec4() = default;
		Vec4(const Vec4&) = default;

		Vec4(float32_t val)
		{
			mValues = _mm_load1_ps(&val);
		}

		Vec4(__m128 val) noexcept
		{
			mValues = val;
		}

		Vec4(float32_t v0, float32_t v1, float32_t v2, float32_t v3) noexcept
		{
			mValues = _mm_set_ps(v3, v2, v1, v0);
		}

		float32_t& operator[](uint16_t index) noexcept
		{
			return (access_as<float32_t*>(&mValues))[index];
		}

		const float32_t& operator[](uint16_t index) const noexcept
		{
			return (access_as<const float32_t*>(&mValues))[index];
		}

		Vec4 operator+(const Vec4& right) const noexcept
		{
			return _mm_add_ps(mValues, right.mValues);
		}

		void operator+=(const Vec4& right) noexcept
		{
			mValues = _mm_add_ps(mValues, right.mValues);
		}

		Vec4 operator-(const Vec4& right) const noexcept
		{
			return _mm_sub_ps(mValues, right.mValues);
		}

		void operator-=(const Vec4& right) noexcept
		{
			mValues = _mm_sub_ps(mValues, right.mValues);
		}

		Vec4 operator*(const Vec4& right) const noexcept
		{
			return _mm_mul_ps(mValues, right.mValues);
		}

		Vec4 operator*(float32_t right) const noexcept
		{
			return _mm_mul_ps(mValues, _mm_load1_ps(&right));
		}

		void operator*=(const Vec4& right) noexcept
		{
			mValues = _mm_mul_ps(mValues, right.mValues);
		}

		void operator*=(float32_t right) noexcept
		{
			mValues = _mm_mul_ps(mValues, _mm_load1_ps(&right));
		}

		Vec4 operator/(const Vec4& right) const noexcept
		{
			return _mm_div_ps(mValues, right.mValues);
		}

		Vec4 operator/(float32_t right) const noexcept
		{
			return _mm_div_ps(mValues, _mm_load1_ps(&right));
		}

		void operator/=(const Vec4& right) noexcept
		{
			mValues = _mm_div_ps(mValues, right.mValues);
		}

		void operator/=(float32_t right) noexcept
		{
			mValues = _mm_div_ps(mValues, _mm_load1_ps(&right));
		}

		Vec4 operator<(const Vec4 &right) const
		{
			return _mm_and_ps(
				_mm_set1_ps(1.0f),
				_mm_cmplt_ps(mValues, right.mValues)
			);
		}

		Vec4 operator<=(const Vec4 &right) const
		{
			return _mm_and_ps(
				_mm_set1_ps(1.0f),
				_mm_cmple_ps(mValues, right.mValues)
			);
		}

		Vec4 operator==(const Vec4 &right) const
		{
			return _mm_and_ps(
				_mm_set1_ps(1.0f),
				_mm_cmpeq_ps(mValues, right.mValues)
			);
		}

		Vec4 operator!=(const Vec4 &right) const
		{
			return _mm_and_ps(
				_mm_set1_ps(1.0f),
				_mm_cmpneq_ps(mValues, right.mValues)
			);
		}

		Vec4 operator>=(const Vec4 &right) const
		{
			return _mm_and_ps(
				_mm_set1_ps(1.0f),
				_mm_cmpge_ps(mValues, right.mValues)
			);
		}

		Vec4 operator>(const Vec4 &right) const
		{
			return _mm_and_ps(
				_mm_set1_ps(1.0f),
				_mm_cmpgt_ps(mValues, right.mValues)
			);
		}

		/**
		 * @brief
		 *  The sum of all components in the Vec3.
		 */
		float32_t sum() const
		{
			return (*this)[0] + (*this)[1] + (*this)[2] + (*this)[3];
		}

		/**
		 * @brief
		 *  The maximum value in the Vec4
		 */
		float32_t max()
		{
			return std::max(
				std::max((*this)[0], (*this)[1]),
				std::max((*this)[2], (*this)[3])
			);
		}

		/**
		 * @brief
		 *  The minimum value in the Vec4
		 */
		float32_t min()
		{
			return std::min(
				std::min((*this)[0], (*this)[1]),
				std::min((*this)[2], (*this)[3])
			);
		}

		int compare(const Vec4& other) const noexcept
		{
			return StdExt::approxCompare(
				(*this)[0], other[0],
				(*this)[1], other[1],
				(*this)[2], other[2],
				(*this)[3], other[3]
			);
		}

		__m128& m128()
		{
			return mValues;
		}

		const __m128& m128() const
		{
			return mValues;
		}
	};

	/**
	 * @brief
	 *  Creates a new Vec4 using two components from A and two components from B,
	 *  using Mask to place components as detailed in mask parameters A0, A1, B0, and B1.
	 *  This results in a Vec4 of { A[A0], A[A1], B[B0], B[B1] }.
	 *
	 * @param A0
	 *	First index into A.
	 * 
	 * @tparam A1
	 *	Second index into A.
	 * 
	 * @tparam B0
	 *	First index into B.
	 * 
	 * @tparam B1
	 *	Second index into B.
	 * 
	 * @tparam vec_t
	 *	Type parameter for the Vec4 that can usually be deduced.
	 *
	 * @see ShuffleMask
	 */
	template<uint32_t A0, uint32_t A1, uint32_t B0, uint32_t B1, VecType vec_t>
	static Vec4<vec_t> shuffle(const Vec4<vec_t>& A, const Vec4<vec_t>& B)
	{
		if constexpr ( std::same_as<float32_t, vec_t> )
		{
			constexpr uint32_t mask = ((B1<<6) | (B0<<4) | (A1<<2) | A0);
			return _mm_shuffle_ps(A.m128(), B.m128(), mask);
		}
		else
		{
			return Vec4<vec_t>(A[A0], A[A1], B[B0], B[B1]);
		}
	}
}

#endif // _ENGINE_VEC_H_