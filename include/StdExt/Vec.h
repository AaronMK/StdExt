#ifndef _STDEXT_VEC_H_
#define _STDEXT_VEC_H_

#include "StdExt.h"

#include "Compare.h"
#include "Utility.h"
#include "Number.h"

#include "Serialize/Binary/Binary.h"
#include "Serialize/Text/Text.h"
#include "Serialize/XML/XML.h"

#ifndef STD_EXT_APPLE
#	ifndef __SSE__
#		define __SSE__
#	endif

#	ifndef __MMX__
#		define __MMX__
#	endif

#	include <xmmintrin.h>
#	include <assert.h>
#else
#	include <simd/simd.h>
#endif

namespace StdExt
{
	template<typename T>
	concept VecType = std::same_as<T, bool> || Arithmetic<T>;

	template<VecType num_t>
	class Vec2
	{
	private:
		num_t mValues[2];

	public:
		constexpr Vec2() = default;
		constexpr Vec2(const Vec2&) = default;

		constexpr Vec2(num_t val)
		{
			mValues[0] = val;
			mValues[1] = val;
		}

		constexpr Vec2(num_t v0, num_t v1) noexcept
		{
			mValues[0] = v0;
			mValues[1] = v1;
		}

		constexpr num_t& operator[](uint16_t index) noexcept
		{
			return mValues[index];
		}

		constexpr const num_t& operator[](uint16_t index) const noexcept
		{
			return mValues[index];
		};

		constexpr Vec2 operator+(const Vec2& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec2(
				mValues[0] + right[0],
				mValues[1] + right[1]
			);
		}

		constexpr Vec2& operator+=(const Vec2& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] += right[0];
			mValues[1] += right[1];
			return *this;
		}

		constexpr Vec2 operator-(const Vec2& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec2(
				mValues[0] - right[0],
				mValues[1] - right[1]
			);
		}

		constexpr Vec2& operator-=(const Vec2& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] -= right[0];
			mValues[1] -= right[1];
			return *this;
		}

		constexpr Vec2 operator*(const Vec2& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec2(
				mValues[0] * right[0],
				mValues[1] * right[1]
			);
		}

		constexpr Vec2 operator*(num_t right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec2(
				mValues[0] * right,
				mValues[1] * right
			);
		}

		constexpr Vec2& operator*=(const Vec2& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] *= right[0];
			mValues[1] *= right[1];
			return *this;
		}

		constexpr Vec2& operator*=(num_t right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] *= right;
			mValues[1] *= right;
			return *this;
		}

		constexpr Vec2 operator/(const Vec2& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec2(
				mValues[0] / right[0],
				mValues[1] / right[1]
			);
		}

		constexpr Vec2 operator/(num_t right) const noexcept
			requires Arithmetic<num_t>
		{
			if constexpr( FloatingPoint<num_t> )
			{
				return (*this) * ( (num_t)1.0 / right);
			}
			else
			{
				return Vec2(
					mValues[0] / right,
					mValues[1] / right
				);
			}
		}

		constexpr Vec2& operator/=(const Vec2& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] /= right[0];
			mValues[1] /= right[1];
			return *this;
		}

		constexpr Vec2& operator/=(num_t right) noexcept
			requires Arithmetic<num_t>
		{
			if constexpr( FloatingPoint<num_t> )
			{
				(*this) *= ( static_cast<num_t>(1.0) / right);
			}
			else
			{
				mValues[0] /= right;
				mValues[1] /= right;
			}
			return *this;
		}

		constexpr auto operator<=>(const Vec2& right) const
		{
			return approxCompare(
				mValues[0], right[0],
				mValues[1], right[1]
			);
		}

		constexpr Vec2 operator<(const Vec2& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec2(
				mValues[0] < right[0],
				mValues[1] < right[1]
			);
		}

		constexpr Vec2 operator<=(const Vec2& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec2(
				mValues[0] <= right[0],
				mValues[1] <= right[1]
			);
		}

		constexpr Vec2 operator==(const Vec2& right) const noexcept
		{
			return Vec2(
				mValues[0] == right[0],
				mValues[1] == right[1]
			);
		}

		constexpr Vec2 operator!=(const Vec2& right) const noexcept
		{
			return Vec2(
				mValues[0] != right[0],
				mValues[1] != right[1]
			);
		}

		constexpr Vec2 operator>=(const Vec2& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec2(
				mValues[0] >= right[0],
				mValues[1] >= right[1]
			);
		}

		constexpr Vec2 operator>(const Vec2& right) const noexcept
			requires Arithmetic<num_t>
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
		constexpr num_t sum() const
		{
			return mValues[0] + mValues[1];
		}

		/**
		 * @brief
		 *  The maximum value in the Vec2
		 */
		constexpr num_t max() const
		{
			return std::max(mValues[0], mValues[1]);
		}

		/**
		 * @brief
		 *  The minimum value in the Vec2
		 */
		constexpr num_t min() const
		{
			return std::min(mValues[0], mValues[1]);
		}
	};
	
	template<VecType num_t>
	class Vec3
	{
	private:
		num_t mValues[3];

	public:
		constexpr Vec3() = default;
		constexpr Vec3(const Vec3&) = default;

		constexpr Vec3(num_t val)
		{
			mValues[0] = val;
			mValues[1] = val;
			mValues[2] = val;
		}

		constexpr Vec3(num_t v0, num_t v1, num_t v2) noexcept
		{
			mValues[0] = v0;
			mValues[1] = v1;
			mValues[2] = v2;
		}

		constexpr num_t& operator[](uint16_t index) noexcept
		{
			return mValues[index];
		}

		constexpr const num_t& operator[](uint16_t index) const noexcept
		{
			return mValues[index];
		};

		constexpr Vec3 operator+(const Vec3& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec3(
				mValues[0] + right[0],
				mValues[1] + right[1],
				mValues[2] + right[2]
			);
		}

		constexpr Vec3& operator+=(const Vec3& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] += right[0];
			mValues[1] += right[1];
			mValues[2] += right[2];
			return *this;
		}

		constexpr Vec3 operator-(const Vec3& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec3(
				mValues[0] - right[0],
				mValues[1] - right[1],
				mValues[2] - right[2]
			);
		}

		constexpr Vec3& operator-=(const Vec3& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] -= right[0];
			mValues[1] -= right[1];
			mValues[2] -= right[2];
			return *this;
		}

		constexpr Vec3 operator*(const Vec3& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec3(
				mValues[0] * right[0],
				mValues[1] * right[1],
				mValues[2] * right[2]
			);
		}

		constexpr Vec3 operator*(num_t right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec3(
				mValues[0] * right,
				mValues[1] * right,
				mValues[2] * right
			);
		}

		constexpr Vec3& operator*=(const Vec3& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] *= right[0];
			mValues[1] *= right[1];
			mValues[2] *= right[2];
			return *this;
		}

		constexpr Vec3& operator*=(num_t right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] *= right;
			mValues[1] *= right;
			mValues[2] *= right;
			return *this;
		}

		constexpr Vec3 operator/(const Vec3& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec3(
				mValues[0] / right[0],
				mValues[1] / right[1],
				mValues[2] / right[2]
			);
		}

		constexpr Vec3 operator/(num_t right) const noexcept
			requires Arithmetic<num_t>
		{
			if constexpr( FloatingPoint<num_t> )
			{
				return (*this) * ( (num_t)1.0 / right);
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

		constexpr Vec3& operator/=(const Vec3& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] /= right[0];
			mValues[1] /= right[1];
			mValues[2] /= right[2];
			return *this;
		}

		constexpr Vec3& operator/=(num_t right) noexcept
			requires Arithmetic<num_t>
		{
			if constexpr( FloatingPoint<num_t> )
			{
				(*this) *= ( (num_t)1.0 / right);
			}
			else
			{
				mValues[0] /= right;
				mValues[1] /= right;
				mValues[2] /= right;
			}
			return *this;
		}

		constexpr Vec3 operator<(const Vec3& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec3(
				mValues[0] < right[0],
				mValues[1] < right[1],
				mValues[2] < right[2]
			);
		}

		constexpr Vec3 operator<=(const Vec3& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec3(
				mValues[0] <= right[0],
				mValues[1] <= right[1],
				mValues[2] <= right[2]
			);
		}

		constexpr Vec3 operator==(const Vec3& right) const noexcept
		{
			return Vec3(
				mValues[0] == right[0],
				mValues[1] == right[1],
				mValues[2] == right[2]
			);
		}

		constexpr Vec3 operator!=(const Vec3& right) const noexcept
		{
			return Vec3(
				mValues[0] != right[0],
				mValues[1] != right[1],
				mValues[2] != right[2]
			);
		}

		constexpr Vec3 operator>=(const Vec3& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec3(
				mValues[0] >= right[0],
				mValues[1] >= right[1],
				mValues[2] >= right[2]
			);
		}

		constexpr Vec3 operator>(const Vec3& right) const noexcept
			requires Arithmetic<num_t>
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
		constexpr num_t sum() const
		{
			return mValues[0] + mValues[1] + mValues[2];
		}

		/**
		 * @brief
		 *  The maximum value in the Vec3
		 */
		constexpr num_t max() const
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
		constexpr num_t min() const
		{
			return std::min(
				std::min(mValues[0], mValues[1]),
				mValues[2]
			);
		}

		constexpr auto operator<=>(const Vec3& other) const noexcept
		{
			return StdExt::approxCompare(
				mValues[0], other[0],
				mValues[1], other[1],
				mValues[2], other[2]
			);
		}
	};
	
	template<VecType num_t>
	class Vec4
	{
	private:
		num_t mValues[4];

	public:
		constexpr Vec4() = default;
		constexpr Vec4(const Vec4&) = default;

		constexpr Vec4(num_t val)
		{
			mValues = {val, val, val, val};
		}

		constexpr Vec4(num_t v0, num_t v1, num_t v2, num_t v3) noexcept
		{
			mValues[0] = v0;
			mValues[1] = v1;
			mValues[2] = v2;
			mValues[3] = v3;
		}

		constexpr num_t& operator[](uint16_t index) noexcept
		{
			return mValues[index];
		}

		constexpr const num_t& operator[](uint16_t index) const noexcept
		{
			return mValues[index];
		};

		constexpr Vec4 operator+(const Vec4& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec4(
				mValues[0] + right[0],
				mValues[1] + right[1],
				mValues[2] + right[2],
				mValues[3] + right[3]
			);
		}

		constexpr Vec4& operator+=(const Vec4& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] += right[0];
			mValues[1] += right[1];
			mValues[2] += right[2];
			mValues[3] += right[3];
			return *this;
		}

		constexpr Vec4 operator-(const Vec4& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec4(
				mValues[0] - right[0],
				mValues[1] - right[1],
				mValues[2] - right[2],
				mValues[3] - right[3]
			);
		}

		constexpr Vec4&  operator-=(const Vec4& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] -= right[0];
			mValues[1] -= right[1];
			mValues[2] -= right[2];
			mValues[3] -= right[3];
			return *this;
		}

		constexpr Vec4 operator*(const Vec4& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec4(
				mValues[0] * right[0],
				mValues[1] * right[1],
				mValues[2] * right[2],
				mValues[3] * right[3]
			);
		}

		constexpr Vec4 operator*(num_t right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec4(
				mValues[0] * right,
				mValues[1] * right,
				mValues[2] * right,
				mValues[3] * right
			);
		}

		constexpr Vec4& operator*=(const Vec4& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] *= right[0];
			mValues[1] *= right[1];
			mValues[2] *= right[2];
			mValues[3] *= right[3];
			return *this;
		}

		constexpr Vec4& operator*=(num_t right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] *= right;
			mValues[1] *= right;
			mValues[2] *= right;
			mValues[3] *= right;
			return *this;
		}

		constexpr Vec4 operator/(const Vec4& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec4(
				mValues[0] / right[0],
				mValues[1] / right[1],
				mValues[2] / right[2],
				mValues[3] / right[3]
			);
		}

		constexpr Vec4 operator/(num_t right) const noexcept
			requires Arithmetic<num_t>
		{
			if constexpr( FloatingPoint<num_t> )
			{
				return (*this) * ( (num_t)1.0 / right);
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

		constexpr Vec4& operator/=(const Vec4& right) noexcept
			requires Arithmetic<num_t>
		{
			mValues[0] /= right[0];
			mValues[1] /= right[1];
			mValues[2] /= right[2];
			mValues[3] /= right[3];
			return *this;
		}

		constexpr Vec4& operator/=(num_t right) noexcept
			requires Arithmetic<num_t>
		{
			if constexpr( FloatingPoint<num_t> )
			{
				(*this) *= ( (num_t)1.0 / right);
			}
			else
			{
				mValues[0] /= right;
				mValues[1] /= right;
				mValues[2] /= right;
				mValues[3] /= right;
			}
			return *this;
		}

		constexpr Vec4 operator<(const Vec4& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec4(
				mValues[0] < right[0],
				mValues[1] < right[1],
				mValues[2] < right[2],
				mValues[3] < right[3]
			);
		}

		constexpr Vec4 operator<=(const Vec4& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec4(
				mValues[0] <= right[0],
				mValues[1] <= right[1],
				mValues[2] <= right[2],
				mValues[3] <= right[3]
			);
		}

		constexpr Vec4 operator==(const Vec4& right) const noexcept
		{
			return Vec4(
				mValues[0] == right[0],
				mValues[1] == right[1],
				mValues[2] == right[2],
				mValues[3] == right[3]
			);
		}

		constexpr Vec4 operator!=(const Vec4& right) const noexcept
		{
			return Vec4(
				mValues[0] != right[0],
				mValues[1] != right[1],
				mValues[2] != right[2],
				mValues[3] != right[3]
			);
		}

		constexpr Vec4 operator>=(const Vec4& right) const noexcept
			requires Arithmetic<num_t>
		{
			return Vec4(
				mValues[0] >= right[0],
				mValues[1] >= right[1],
				mValues[2] >= right[2],
				mValues[3] >= right[3]
			);
		}

		constexpr Vec4 operator>(const Vec4& right) const noexcept
			requires Arithmetic<num_t>
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
		constexpr num_t sum() const
		{
			return mValues[0] + mValues[1] + mValues[2] + mValues[3];
		}

		/**
		 * @brief
		 *  The maximum value in the Vec4
		 */
		constexpr num_t max() const
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
		constexpr num_t min() const
		{
			return std::min(
				std::min(mValues[0], mValues[1]),
				std::min(mValues[2], mValues[3])
			);
		}

		constexpr auto operator<=>(const Vec4& other) const noexcept
		{
			return approxCompare(
				mValues[0], other[0],
				mValues[1], other[1],
				mValues[2], other[2],
				mValues[3], other[3]
			);
		}
	};

#ifndef STD_EXT_APPLE
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

		Vec4& operator+=(const Vec4& right) noexcept
		{
			mValues = _mm_add_ps(mValues, right.mValues);
			return *this;
		}

		Vec4 operator-(const Vec4& right) const noexcept
		{
			return _mm_sub_ps(mValues, right.mValues);
		}

		Vec4& operator-=(const Vec4& right) noexcept
		{
			mValues = _mm_sub_ps(mValues, right.mValues);
			return *this;
		}

		Vec4 operator*(const Vec4& right) const noexcept
		{
			return _mm_mul_ps(mValues, right.mValues);
		}

		Vec4 operator*(float32_t right) const noexcept
		{
			return _mm_mul_ps(mValues, _mm_load1_ps(&right));
		}

		Vec4& operator*=(const Vec4& right) noexcept
		{
			mValues = _mm_mul_ps(mValues, right.mValues);
			return *this;
		}

		Vec4& operator*=(float32_t right) noexcept
		{
			mValues = _mm_mul_ps(mValues, _mm_load1_ps(&right));
			return *this;
		}

		Vec4 operator/(const Vec4& right) const noexcept
		{
			return _mm_div_ps(mValues, right.mValues);
		}

		Vec4 operator/(float32_t right) const noexcept
		{
			return _mm_div_ps(mValues, _mm_load1_ps(&right));
		}

		Vec4& operator/=(const Vec4& right) noexcept
		{
			mValues = _mm_div_ps(mValues, right.mValues);
			return *this;
		}

		Vec4& operator/=(float32_t right) noexcept
		{
			mValues = _mm_div_ps(mValues, _mm_load1_ps(&right));
			return *this;
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
		float32_t max() const
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
		float32_t min() const
		{
			return std::min(
				std::min((*this)[0], (*this)[1]),
				std::min((*this)[2], (*this)[3])
			);
		}

		constexpr auto operator<=>(const Vec4& other) const noexcept
		{
			return StdExt::approxCompare(
				(*this)[0], other[0],
				(*this)[1], other[1],
				(*this)[2], other[2],
				(*this)[3], other[3]
			);
		}

		__m128& nativeVec4()
		{
			return mValues;
		}

		const __m128& nativeVec4() const
		{
			return mValues;
		}
	};
#endif

#if 0
	template<>
	class Vec4<float32_t>
	{
	private:
		simd_float4 mValues;

	public:
		constexpr Vec4() = default;
		constexpr Vec4(const Vec4&) = default;

		constexpr Vec4(float32_t val)
		{
			mValues = simd::make_float4(val, val, val, val);
		}

		constexpr Vec4(simd_float4 val) noexcept
		{
			mValues = val;
		}

		constexpr Vec4(float32_t v0, float32_t v1, float32_t v2, float32_t v3) noexcept
		{
			mValues = simd::make_float4(v0, v1, v2, v3);
		}

		constexpr float32_t& operator[](uint16_t index) noexcept
		{
			return mValues[index];
		}

		constexpr const float32_t operator[](uint16_t index) const noexcept
		{
			return mValues[index];
		}

		Vec4 operator+(const Vec4& right) const noexcept
		{
			return mValues + right.mValues;
		}

		Vec4& operator+=(const Vec4& right) noexcept
		{
			mValues += right.mValues;
			return *this;
		}

		Vec4 operator-(const Vec4& right) const noexcept
		{
			return mValues - right.mValues;
		}

		Vec4& operator-=(const Vec4& right) noexcept
		{
			mValues -= right.mValues;
			return *this;
		}

		Vec4 operator*(const Vec4& right) const noexcept
		{
			return mValues * right.mValues;
		}

		Vec4 operator*(float32_t right) const noexcept
		{
			return mValues * Vec4(right).mValues;
		}

		Vec4& operator*=(const Vec4& right) noexcept
		{
			mValues *= right.mValues;
			return *this;
		}

		Vec4& operator*=(float32_t right) noexcept
		{
			mValues = mValues * Vec4(right).mValues;
			return *this;
		}

		constexpr Vec4 operator/(const Vec4& right) const noexcept
		{
			return mValues / right.mValues;
		}

		constexpr Vec4 operator/(float32_t right) const noexcept
		{
			return mValues / Vec4(right).mValues;
		}

		Vec4& operator/=(const Vec4& right) noexcept
		{
			mValues = mValues / right.mValues;
			return *this;
		}

		Vec4& operator/=(float32_t right) noexcept
		{
			mValues = mValues / Vec4(right).mValues;
			return *this;
		}

		constexpr Vec4 operator<(const Vec4 &right) const
		{
			return Vec4(
				mValues[0] < right.mValues[0],
				mValues[1] < right.mValues[1],
				mValues[2] < right.mValues[2],
				mValues[3] < right.mValues[3]
			);
		}

		constexpr Vec4 operator<=(const Vec4 &right) const
		{
			return Vec4(
				mValues[0] <= right.mValues[0],
				mValues[1] <= right.mValues[1],
				mValues[2] <= right.mValues[2],
				mValues[3] <= right.mValues[3]
			);
		}

		constexpr Vec4 operator==(const Vec4 &right) const
		{
			return Vec4(
				mValues[0] == right.mValues[0],
				mValues[1] == right.mValues[1],
				mValues[2] == right.mValues[2],
				mValues[3] == right.mValues[3]
			);
		}

		constexpr Vec4 operator!=(const Vec4 &right) const
		{
			return Vec4(
				mValues[0] != right.mValues[0],
				mValues[1] != right.mValues[1],
				mValues[2] != right.mValues[2],
				mValues[3] != right.mValues[3]
			);
		}

		constexpr Vec4 operator>=(const Vec4 &right) const
		{
			return Vec4(
				mValues[0] >= right.mValues[0],
				mValues[1] >= right.mValues[1],
				mValues[2] >= right.mValues[2],
				mValues[3] >= right.mValues[3]
			);
		}

		constexpr Vec4 operator>(const Vec4 &right) const
		{
			return Vec4(
				mValues[0] > right.mValues[0],
				mValues[1] > right.mValues[1],
				mValues[2] > right.mValues[2],
				mValues[3] > right.mValues[3]
			);
		}

		/**
		 * @brief
		 *  The sum of all components in the Vec3.
		 */
		constexpr float32_t sum() const
		{
			return (*this)[0] + (*this)[1] + (*this)[2] + (*this)[3];
		}

		/**
		 * @brief
		 *  The maximum value in the Vec4
		 */
		constexpr float32_t max() const
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
		constexpr float32_t min() const
		{
			return std::min(
				std::min((*this)[0], (*this)[1]),
				std::min((*this)[2], (*this)[3])
			);
		}

		constexpr auto operator<=>(const Vec4& other) const noexcept
		{
			return StdExt::approxCompare(
				(*this)[0], other[0],
				(*this)[1], other[1],
				(*this)[2], other[2],
				(*this)[3], other[3]
			);
		}

		simd_float4& nativeVec4()
		{
			return mValues;
		}

		const simd_float4& nativeVec4() const
		{
			return mValues;
		}
	};
#endif // !STD_EXT_APPLE

	/**
	 * @brief
	 *  Creates a new Vec4 using two components from A and two components from B,
	 *  using Mask to place components as detailed in mask parameters A0, A1, B0, and B1.
	 *  This results in a Vec4 of { A[A0], A[A1], B[B0], B[B1] }.
	 *
	 * @tparam A0
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
	 * @tparam num_t
	 *	Type parameter for the Vec4 that can usually be deduced.
	 *
	 * @see ShuffleMask
	 */
	template<uint32_t A0, uint32_t A1, uint32_t B0, uint32_t B1, VecType num_t>
	static Vec4<num_t> shuffle(const Vec4<num_t>& A, const Vec4<num_t>& B)
	{
		if constexpr ( std::same_as<float32_t, num_t> && !Platform::Compiler::isApple )
		{
			constexpr uint32_t mask = ((B1<<6) | (B0<<4) | (A1<<2) | A0);
			return _mm_shuffle_ps(A.nativeVec4(), B.nativeVec4(), mask);
		}
		else
		{
			return Vec4<num_t>(A[A0], A[A1], B[B0], B[B1]);
		}
	}

	namespace Serialize::Binary
	{
		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<bool>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<bool>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<uint8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<uint16_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<uint32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<uint64_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<int8_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<int16_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<int32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<int64_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<float32_t>* out);
		
		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec2<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec2<float64_t>& val);

		//////////////////////////////////////

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<bool>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<bool>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<uint16_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<uint32_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<uint64_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<int8_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<int16_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<int32_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<int64_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<float32_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec3<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec3<float64_t>& val);

		//////////////////////////////////////

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<bool>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<bool>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<uint16_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<uint32_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<uint64_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<int8_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<int16_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<int32_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<int64_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<float32_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, Vec4<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const Vec4<float64_t>& val);
	}

	namespace Serialize::XML
	{
		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<bool>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<bool>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<uint16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<uint32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<uint64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<int8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<int16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<int32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<int64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<float32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec2<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec2<float64_t>& val);

		//////////////////////////////////////

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<bool>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<bool>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<uint16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<uint32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<uint64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<int8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<int16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<int32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<int64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<float32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec3<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec3<float64_t>& val);

		//////////////////////////////////////

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<bool>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<bool>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<uint8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<uint8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<uint16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<uint16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<uint32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<uint32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<uint64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<uint64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<int8_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<int8_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<int16_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<int16_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<int32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<int32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<int64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<int64_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<float32_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<float32_t>& val);

		template<>
		STD_EXT_EXPORT void read(const Element& element, Vec4<float64_t>* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Vec4<float64_t>& val);
	}
}

#endif // _STDEXT_VEC_H_