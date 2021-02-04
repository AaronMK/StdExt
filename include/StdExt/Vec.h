#ifndef _ENGINE_VEC_H_
#define _ENGINE_VEC_H_

#include "StdExt.h"
#include "Concepts.h"
#include "Utility.h"

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
		Vec2() = default;
		Vec2(const Vec2&) = default;

		Vec2(vec_t val)
		{
			mValues = {val, val};
		}

		Vec2(vec_t v0, vec_t v1) noexcept
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

		Vec2<bool> operator<(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2<bool>(
				mValues[0] < right[0],
				mValues[1] < right[1]
			);
		}

		Vec2<bool> operator<=(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2<bool>(
				mValues[0] <= right[0],
				mValues[1] <= right[1]
			);
		}

		Vec2<bool> operator==(const Vec2& right) const noexcept
		{
			return Vec2<bool>(
				mValues[0] == right[0],
				mValues[1] == right[1]
			);
		}

		Vec2<bool> operator!=(const Vec2& right) const noexcept
		{
			return Vec2<bool>(
				mValues[0] != right[0],
				mValues[1] != right[1]
			);
		}
		Vec2<bool> operator>=(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2<bool>(
				mValues[0] >= right[0],
				mValues[1] >= right[1]
			);
		}

		Vec2<bool> operator>(const Vec2& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec2<bool>(
				mValues[0] > right[0],
				mValues[1] > right[1]
			);
		}

		int compare(const Vec2& other) const noexcept
		{
			return StdExt::compare(
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
		Vec3() = default;
		Vec3(const Vec3&) = default;

		Vec3(vec_t val)
		{
			mValues = {val, val, val};
		}

		Vec3(vec_t v0, vec_t v1, vec_t v2) noexcept
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

		Vec3<bool> operator<(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3<bool>(
				mValues[0] < right[0],
				mValues[1] < right[1],
				mValues[2] < right[2]
			);
		}

		Vec3<bool> operator<=(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3<bool>(
				mValues[0] <= right[0],
				mValues[1] <= right[1],
				mValues[2] <= right[2]
			);
		}

		Vec3<bool> operator==(const Vec3& right) const noexcept
		{
			return Vec3<bool>(
				mValues[0] == right[0],
				mValues[1] == right[1],
				mValues[2] == right[2]
			);
		}

		Vec3<bool> operator!=(const Vec3& right) const noexcept
		{
			return Vec3<bool>(
				mValues[0] != right[0],
				mValues[1] != right[1],
				mValues[2] != right[2]
			);
		}

		Vec3<bool> operator>=(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3<bool>(
				mValues[0] >= right[0],
				mValues[1] >= right[1],
				mValues[2] >= right[2]
			);
		}

		Vec3<bool> operator>(const Vec3& right) const noexcept
			requires Arithmetic<vec_t>
		{
			return Vec3<bool>(
				mValues[0] > right[0],
				mValues[1] > right[1],
				mValues[2] > right[2]
			);
		}

		int compare(const Vec3& other) const noexcept
		{
			return StdExt::compare(
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
		Vec4() = default;
		Vec4(const Vec4&) = default;

		Vec4(vec_t val)
		{
			mValues = {val, val, val, val};
		}

		Vec4(vec_t v0, vec_t v1, vec_t v2, vec_t v3) noexcept
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

		Vec4<bool> operator<(const Vec4& right) const
			requires Arithmetic<vec_t>
		{
			return Vec4<bool>(
				mValues[0] < right[0],
				mValues[1] < right[1],
				mValues[2] < right[2],
				mValues[3] < right[3]
			);
		}

		Vec4<bool> operator<=(const Vec4& right) const
			requires Arithmetic<vec_t>
		{
			return Vec4<bool>(
				mValues[0] <= right[0],
				mValues[1] <= right[1],
				mValues[2] <= right[2],
				mValues[3] <= right[3]
			);
		}

		Vec4<bool> operator==(const Vec4& right) const
		{
			return Vec4<bool>(
				mValues[0] == right[0],
				mValues[1] == right[1],
				mValues[2] == right[2],
				mValues[3] == right[3]
			);
		}

		Vec4<bool> operator!=(const Vec4& right) const
		{
			return Vec4<bool>(
				mValues[0] != right[0],
				mValues[1] != right[1],
				mValues[2] != right[2],
				mValues[3] != right[3]
			);
		}

		Vec4<bool> operator>=(const Vec4& right) const
			requires Arithmetic<vec_t>
		{
			return Vec4<bool>(
				mValues[0] >= right[0],
				mValues[1] >= right[1],
				mValues[2] >= right[2],
				mValues[3] >= right[3]
			);
		}

		Vec4<bool> operator>(const Vec4& right) const
			requires Arithmetic<vec_t>
		{
			return Vec4<bool>(
				mValues[0] > right[0],
				mValues[1] > right[1],
				mValues[2] > right[2],
				mValues[3] > right[3]
			);
		}

		int compare(const Vec4& other) const noexcept
		{
			return StdExt::compare(
				mValues[0], other[0],
				mValues[1], other[1],
				mValues[2], other[2],
				mValues[3], other[3]
			);
		}
	};
}

#endif // _ENGINE_VEC_H_