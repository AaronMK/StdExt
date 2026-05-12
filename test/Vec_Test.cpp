#include <StdExt/Vec.h>
#include <StdExt/Const/Vec.h>

#include <StdExt/Utility.h>
#include <StdExt/Concepts.h>

#include <StdExt/Test/Test.h>

#include "SerializeTesting.h"

using namespace StdExt;
using namespace StdExt::Test;

template <Arithmetic T>
static T test_random(T min, T max)
{
	T ret = rand<T>(min, max);

	while( static_cast<T>(0) == ret )
		ret = rand<T>(min, max);

	return ret;
}

template<VecType num_t>
void test_vec2()
{
	std::string type_str{ typeid(num_t).name() };
	std::function<Vec2<num_t>()> r;

	if constexpr ( Unsigned<num_t> )
		r = []() { return Vec2<num_t>(test_random(10, 20), test_random(10, 20)); };
	else if constexpr ( FloatingPoint<num_t> )
		r = []() { return Vec2<num_t>(test_random(-10.0f, 10.0f), test_random(-10.0f, 10.0f)); };
	else
		r = []() { return Vec2<num_t>(test_random(-10, 10), test_random(-10, 10)); };

	Vec2<num_t> left = r();
	Vec2<num_t> right = r();

	uint32_t index_same = rand<uint32_t>(0, 1);
	left[index_same] = right[index_same];

	testForResult(
		"Vec2 comparison ( < )",
		(left < right), Vec2<num_t>(left[0] < right[0], left[1] < right[1])
	);

	testForResult(
		"Vec2 comparison ( <= )",
		(left <= right), Vec2<num_t>(left[0] <= right[0], left[1] <= right[1])
	);

	testForResult(
		"Vec2 comparison ( == )",
		(left == right), Vec2<num_t>(left[0] == right[0], left[1] == right[1])
	);

	testForResult(
		"Vec comparison ( != )",
		(left != right),  Vec2<num_t>(left[0] != right[0], left[1] != right[1])
	);

	testForResult(
		"Vec2 comparison ( >= )",
		(left >= right), Vec2<num_t>(left[0] >= right[0], left[1] >= right[1])
	);

	testForResult(
		"Vec2 comparison ( > )",
		(left > right), Vec2<num_t>(left[0] > right[0], left[1] > right[1])
	);

	testForResult(
		"Vec2 operator ( + )",
		(left + right), Vec2<num_t>(left[0] + right[0], left[1] + right[1])
	);

	testForResult(
		"Vec2 operator ( - )",
		(left - right), Vec2<num_t>(left[0] - right[0], left[1] - right[1])
	);

	testForResult(
		"Vec2 operator ( * )",
		(left * right), Vec2<num_t>(left[0] * right[0], left[1] * right[1])
	);

	testForResult(
		"Vec2 operator ( / )",
		(left / right), Vec2<num_t>(left[0] / right[0], left[1] / right[1])
	);

	Vec2<num_t> modified = left;
	modified += right;

	testForResult(
		"Vec2 operator ( += )",
		(left + right), modified
	);

	modified = left;
	modified -= right;

	testForResult(
		"Vec2 operator ( -= )",
		(left - right), modified
	);

	modified = left;
	modified *= right;

	testForResult(
		"Vec2 operator ( *= )",
		(left * right), modified
	);

	modified = left;
	modified /= right;

	testForResult(
		"Vec2 operator ( /= )",
		(left / right), modified
	);

	testBinarySerialize(r());
	testXmlSerialize(r());
}

template<VecType num_t>
void test_vec3()
{
	std::string type_str{ typeid(num_t).name() };
	std::function<Vec3<num_t>()> r;

	if constexpr ( Unsigned<num_t> )
		r = []() { return Vec3<num_t>(test_random(10, 20), test_random(10, 20), test_random(10, 20)); };
	else if constexpr ( FloatingPoint<num_t> )
		r = []() { return Vec3<num_t>(test_random(-10.0f, 10.0f), test_random(-10.0f, 10.0f), test_random(-10.0f, 10.0f)); };
	else
		r = []() { return Vec3<num_t>(test_random(-10, 10), test_random(-10, 10), test_random(-10, 10)); };

	Vec3<num_t> left = r();
	Vec3<num_t> right = r();

	uint32_t index_same = rand<uint32_t>(0, 2);
	left[index_same] = right[index_same];

	testForResult(
		"Vec3 comparison ( < )",
		(left < right), Vec3<num_t>(left[0] < right[0], left[1] < right[1], left[2] < right[2])
	);

	testForResult(
		"Vec3 comparison ( <= )",
		(left <= right), Vec3<num_t>(left[0] <= right[0], left[1] <= right[1], left[2] <= right[2])
	);

	testForResult(
		"Vec3 comparison ( == )",
		(left == right), Vec3<num_t>(left[0] == right[0], left[1] == right[1], left[2] == right[2])
	);

	testForResult(
		"Vec comparison ( != )",
		(left != right), Vec3<num_t>(left[0] != right[0], left[1] != right[1], left[2] != right[2])
	);

	testForResult(
		"Vec3 comparison ( >= )",
		(left >= right), Vec3<num_t>(left[0] >= right[0], left[1] >= right[1], left[2] >= right[2])
	);

	testForResult(
		"Vec3 comparison ( > )",
		(left > right), Vec3<num_t>(left[0] > right[0], left[1] > right[1], left[2] > right[2])
	);

	testForResult(
		"Vec3 operator ( + )",
		(left + right), Vec3<num_t>(left[0] + right[0], left[1] + right[1], left[2] + right[2])
	);

	testForResult(
		"Vec3 operator ( - )",
		(left - right), Vec3<num_t>(left[0] - right[0], left[1] - right[1], left[2] - right[2])
	);

	testForResult(
		"Vec3 operator ( * )",
		(left * right), Vec3<num_t>(left[0] * right[0], left[1] * right[1], left[2] * right[2])
	);

	testForResult(
		"Vec3 operator ( / )",
		(left / right), Vec3<num_t>(left[0] / right[0], left[1] / right[1], left[2] / right[2])
	);

	Vec3<num_t> modified = left;
	modified += right;

	testForResult(
		"Vec3 operator ( += )",
		(left + right), modified
	);

	modified = left;
	modified -= right;

	testForResult(
		"Vec3 operator ( -= )",
		(left - right), modified
	);

	modified = left;
	modified *= right;

	testForResult(
		"Vec3 operator ( *= )",
		(left * right), modified
	);

	modified = left;
	modified /= right;

	testForResult(
		"Vec3 operator ( /= )",
		(left / right), modified
	);

	testBinarySerialize(r());
	testXmlSerialize(r());
}

template<VecType num_t>
void test_vec4()
{
	std::string type_str{ typeid(num_t).name() };
	std::function<Vec4<num_t>()> r;

	if constexpr ( Unsigned<num_t> )
		r = []() { return Vec4<num_t>(test_random(10, 20), test_random(10, 20), test_random(10, 20), test_random(10, 20)); };
	else if constexpr ( FloatingPoint<num_t> )
		r = []() { return Vec4<num_t>(test_random(-10.0f, 10.0f), test_random(-10.0f, 10.0f), test_random(-10.0f, 10.0f), test_random(-10.0f, 10.0f)); };
	else
		r = []() { return Vec4<num_t>(test_random(-10, 10), test_random(-10, 10), test_random(-10, 10), test_random(-10, 10)); };

	Vec4<num_t> left = r();
	Vec4<num_t> right = r();

	uint32_t index_same = rand<uint32_t>(0, 3);
	left[index_same] = right[index_same];

	testForResult(
		"Vec4 comparison ( < )",
		(left < right), Vec4<num_t>(left[0] < right[0], left[1] < right[1], left[2] < right[2], left[3] < right[3])
	);

	testForResult(
		"Vec4 comparison ( <= )",
		(left <= right), Vec4<num_t>(left[0] <= right[0], left[1] <= right[1], left[2] <= right[2], left[3] <= right[3])
	);

	testForResult(
		"Vec4 comparison ( == )",
		(left == right), Vec4<num_t>(left[0] == right[0], left[1] == right[1], left[2] == right[2], left[3] == right[3])
	);

	testForResult(
		"Vec comparison ( != )",
		(left != right), Vec4<num_t>(left[0] != right[0], left[1] != right[1], left[2] != right[2], left[3] != right[3])
	);

	testForResult(
		"Vec4 comparison ( >= )",
		(left >= right), Vec4<num_t>(left[0] >= right[0], left[1] >= right[1], left[2] >= right[2], left[3] >= right[3])
	);

	testForResult(
		"Vec4 comparison ( > )",
		(left > right), Vec4<num_t>(left[0] > right[0], left[1] > right[1], left[2] > right[2], left[3] > right[3])
	);

	testForResult(
		"Vec4 operator ( + )",
		(left + right), Vec4<num_t>(left[0] + right[0], left[1] + right[1], left[2] + right[2], left[3] + right[3])
	);

	testForResult(
		"Vec4 operator ( - )",
		(left - right), Vec4<num_t>(left[0] - right[0], left[1] - right[1], left[2] - right[2], left[3] - right[3])
	);

	testForResult(
		"Vec4 operator ( * )",
		(left * right), Vec4<num_t>(left[0] * right[0], left[1] * right[1], left[2] * right[2], left[3] * right[3])
	);

	testForResult(
		"Vec4 operator ( / )",
		(left / right), Vec4<num_t>(left[0] / right[0], left[1] / right[1], left[2] / right[2], left[3] / right[3])
	);

	Vec4<num_t> modified = left;
	modified += right;

	testForResult(
		"Vec4 operator ( += )",
		(left + right), modified
	);

	modified = left;
	modified -= right;

	testForResult(
		"Vec4 operator ( -= )",
		(left - right), modified
	);

	modified = left;
	modified *= right;

	testForResult(
		"Vec4 operator ( *= )",
		(left * right), modified
	);

	modified = left;
	modified /= right;

	testForResult(
		"Vec4 operator ( /= )",
		(left / right), modified
	);

	testForResult(
		"Shuffle using mask 1",
		shuffle<0, 2, 1, 3>(left, right), Vec4<num_t>(left[0], left[2], right[1], right[3])
	);

	testForResult(
		"Shuffle using mask 2",
		shuffle<3, 1, 2, 0>(left, right), Vec4<num_t>(left[3], left[1], right[2], right[0])
	);

	testBinarySerialize(r());
	testXmlSerialize(r());
}

template<typename T, typename U, size_t Width>
void test_const_vec_mixed()
{
	using TVec = Const::Vec<T, Width>;
	using UVec = Const::Vec<U, Width>;

	auto make_tvec = [&]() -> TVec
		{
			return [&]<size_t... I>(std::index_sequence<I...>) -> TVec {
				if constexpr ( Unsigned<T> )
					return TVec{ ((void)I, test_random(10, 20))... };
				else if constexpr ( FloatingPoint<T> )
					return TVec{ ((void)I, test_random(-10.0f, 10.0f))... };
				else
					return TVec{ ((void)I, test_random(-10, 10))... };
			}(std::make_index_sequence<Width>{});
		};

	auto make_uvec = [&]() -> UVec
		{
			return [&]<size_t... I>(std::index_sequence<I...>) -> UVec {
				if constexpr ( Unsigned<U> )
					return UVec{ ((void)I, test_random(10, 20))... };
				else if constexpr ( FloatingPoint<U> )
					return UVec{ ((void)I, test_random(-10.0f, 10.0f))... };
				else
					return UVec{ ((void)I, test_random(-10, 10))... };
			}(std::make_index_sequence<Width>{});
		};

	TVec left  = make_tvec();
	UVec right = make_uvec();

	// Build a Vec of the natural result type of a binary op on T and U.
	auto elem_op = [&](auto fn) 
		{
			using result_t = decltype(fn(left[0], right[0]));
			return [&, fn]<size_t... I>(std::index_sequence<I...>) {
				return Const::Vec<result_t, Width>{ fn(left[I], right[I])... };
			}(std::make_index_sequence<Width>{});
		};

	// Build a TVec from a binary op, casting results back to T (for assignment operators).
	auto elem_assign = [&](auto fn) -> TVec {
		return [&, fn]<size_t... I>(std::index_sequence<I...>) -> TVec {
			return TVec{ static_cast<T>(fn(left[I], right[I]))... };
		}(std::make_index_sequence<Width>{});
	};

	// Mixed-type comparisons
	testForResult(
		"Const::Vec mixed comparison ( < )",
		left < right,
		elem_op([](T l, U r) -> bool { return l < r; })
	);

	testForResult(
		"Const::Vec mixed comparison ( <= )",
		left <= right,
		elem_op([](T l, U r) -> bool { return l <= r; })
	);

	testForResult(
		"Const::Vec mixed comparison ( == )",
		left == right,
		elem_op([](T l, U r) -> bool { return l == r; })
	);

	testForResult(
		"Const::Vec mixed comparison ( != )",
		left != right,
		elem_op([](T l, U r) -> bool { return l != r; })
	);

	testForResult(
		"Const::Vec mixed comparison ( >= )",
		left >= right,
		elem_op([](T l, U r) -> bool { return l >= r; })
	);

	testForResult(
		"Const::Vec mixed comparison ( > )",
		left > right,
		elem_op([](T l, U r) -> bool { return l > r; })
	);

	// Mixed-type arithmetic
	testForResult(
		"Const::Vec mixed operator ( + )",
		left + right,
		elem_op([](T l, U r) { return l + r; })
	);

	testForResult(
		"Const::Vec mixed operator ( - )",
		left - right,
		elem_op([](T l, U r) { return l - r; })
	);

	testForResult(
		"Const::Vec mixed operator ( * )",
		left * right,
		elem_op([](T l, U r) { return l * r; })
	);

	testForResult(
		"Const::Vec mixed operator ( / )",
		left / right,
		elem_op([](T l, U r) { return l / r; })
	);

	// Mixed-type assignment operators
	TVec modified = left;
	modified += right;
	testForResult("Const::Vec mixed operator ( += )", modified, elem_assign([](T l, U r) { return l + r; }));

	modified = left;
	modified -= right;
	testForResult("Const::Vec mixed operator ( -= )", modified, elem_assign([](T l, U r) { return l - r; }));

	modified = left;
	modified *= right;
	testForResult("Const::Vec mixed operator ( *= )", modified, elem_assign([](T l, U r) { return l * r; }));

	modified = left;
	modified /= right;
	testForResult("Const::Vec mixed operator ( /= )", modified, elem_assign([](T l, U r) { return l / r; }));

	// ApplyOp with a custom functor: fused multiply-add (l * r + r)
	auto custom_result = left.template ApplyOp<U, [](T l, U r) {
		using common_t = std::common_type_t<T, U>;
		return static_cast<common_t>(l) * static_cast<common_t>(r) + static_cast<common_t>(r);
	}>(right);

	testForResult(
		"Const::Vec mixed ApplyOp (custom functor)",
		custom_result,
		elem_op([](T l, U r) {
			using common_t = std::common_type_t<T, U>;
			return static_cast<common_t>(l) * static_cast<common_t>(r) + static_cast<common_t>(r);
		})
	);

	// Broadcast construction: build a TVec from a single U value
	U bval = make_uvec()[0];
	TVec broadcast(bval);
	testByCheck(
		"Const::Vec broadcast from convertible type",
		[&]() {
			for (size_t i = 0; i < Width; ++i)
				if ( broadcast[i] != static_cast<T>(bval) )
					return false;
			return true;
		}
	);

	// Swizzle result from a mixed op and verify element access
	auto sum_vec = left + right;
	testForResult(
		"Const::Vec mixed swizzle after op",
		sum_vec.template swizzle<0, Width - 1>(),
		Const::Vec<decltype(left[0] + right[0]), 2>{ sum_vec[0], sum_vec[Width - 1] }
	);

	// Lexicographic <=> between two TVecs (result of independent ops)
	testByCheck(
		"Const::Vec mixed operator <=> (reflexive)",
		[&]() { return std::is_eq(left <=> left); }
	);
}

template<typename T, size_t Width>
void test_const_vec()
{
	using CVec = Const::Vec<T, Width>;

	auto make_vec = [&]() -> CVec {
		return [&]<size_t... I>(std::index_sequence<I...>) -> CVec {
			if constexpr ( Unsigned<T> )
				return CVec{ ((void)I, test_random(10, 20))... };
			else if constexpr ( FloatingPoint<T> )
				return CVec{ ((void)I, test_random(-10.0f, 10.0f))... };
			else
				return CVec{ ((void)I, test_random(-10, 10))... };
		}(std::make_index_sequence<Width>{});
	};

	CVec left  = make_vec();
	CVec right = make_vec();

	uint32_t index_same = rand<uint32_t>(0, Width - 1);
	left[index_same] = right[index_same];

	// Build a Vec of the binary op's natural result type (may differ from T due to integer promotion).
	auto elem_op = [&](auto fn) {
		using result_t = decltype(fn(left[0], right[0]));
		return [&, fn]<size_t... I>(std::index_sequence<I...>) {
			return Const::Vec<result_t, Width>{ fn(left[I], right[I])... };
		}(std::make_index_sequence<Width>{});
	};

	// Build a CVec from a binary op, casting back to T (for validating assignment operators).
	auto elem_assign = [&](auto fn) -> CVec {
		return [&, fn]<size_t... I>(std::index_sequence<I...>) -> CVec {
			return CVec{ static_cast<T>(fn(left[I], right[I]))... };
		}(std::make_index_sequence<Width>{});
	};

	testForResult(
		"Const::Vec comparison ( < )",
		left < right,
		elem_op([](T l, T r) -> bool { return l < r; })
	);

	testForResult(
		"Const::Vec comparison ( <= )",
		left <= right,
		elem_op([](T l, T r) -> bool { return l <= r; })
	);

	testForResult(
		"Const::Vec comparison ( == )",
		left == right,
		elem_op([](T l, T r) -> bool { return l == r; })
	);

	testForResult(
		"Const::Vec comparison ( != )",
		left != right,
		elem_op([](T l, T r) -> bool { return l != r; })
	);

	testForResult(
		"Const::Vec comparison ( >= )",
		left >= right,
		elem_op([](T l, T r) -> bool { return l >= r; })
	);

	testForResult(
		"Const::Vec comparison ( > )",
		left > right,
		elem_op([](T l, T r) -> bool { return l > r; })
	);

	testForResult(
		"Const::Vec operator ( + )",
		left + right,
		elem_op([](T l, T r) { return l + r; })
	);

	testForResult(
		"Const::Vec operator ( - )",
		left - right,
		elem_op([](T l, T r) { return l - r; })
	);

	testForResult(
		"Const::Vec operator ( * )",
		left * right,
		elem_op([](T l, T r) { return l * r; })
	);

	testForResult(
		"Const::Vec operator ( / )",
		left / right,
		elem_op([](T l, T r) { return l / r; })
	);

	CVec modified = left;
	modified += right;
	testForResult("Const::Vec operator ( += )", modified, elem_assign([](T l, T r) { return l + r; }));

	modified = left;
	modified -= right;
	testForResult("Const::Vec operator ( -= )", modified, elem_assign([](T l, T r) { return l - r; }));

	modified = left;
	modified *= right;
	testForResult("Const::Vec operator ( *= )", modified, elem_assign([](T l, T r) { return l * r; }));

	modified = left;
	modified /= right;
	testForResult("Const::Vec operator ( /= )", modified, elem_assign([](T l, T r) { return l / r; }));

	if constexpr ( !FloatingPoint<T> ) {
		testForResult(
			"Const::Vec operator ( % )",
			left % right,
			elem_op([](T l, T r) { return l % r; })
		);

		modified = left;
		modified %= right;
		testForResult("Const::Vec operator ( %= )", modified, elem_assign([](T l, T r) { return l % r; }));

		testForResult(
			"Const::Vec operator ( & )",
			left & right,
			elem_op([](T l, T r) { return l & r; })
		);

		testForResult(
			"Const::Vec operator ( | )",
			left | right,
			elem_op([](T l, T r) { return l | r; })
		);

		testForResult(
			"Const::Vec operator ( ^ )",
			left ^ right,
			elem_op([](T l, T r) { return l ^ r; })
		);

		modified = left;
		modified &= right;
		testForResult("Const::Vec operator ( &= )", modified, elem_assign([](T l, T r) { return l & r; }));

		modified = left;
		modified |= right;
		testForResult("Const::Vec operator ( |= )", modified, elem_assign([](T l, T r) { return l | r; }));

		modified = left;
		modified ^= right;
		testForResult("Const::Vec operator ( ^= )", modified, elem_assign([](T l, T r) { return l ^ r; }));

		testForResult(
			"Const::Vec operator ( ~ )",
			~left,
			[&]<size_t... I>(std::index_sequence<I...>) -> CVec {
				return CVec{ static_cast<T>(~left[I])... };
			}(std::make_index_sequence<Width>{})
		);
	}

	if constexpr ( !Unsigned<T> ) {
		testForResult(
			"Const::Vec unary operator ( - )",
			-left,
			[&]<size_t... I>(std::index_sequence<I...>) -> CVec {
				return CVec{ static_cast<T>(-left[I])... };
			}(std::make_index_sequence<Width>{})
		);
	}

	testForResult(
		"Const::Vec swizzle (compile-time)",
		left.template swizzle<0, Width - 1>(),
		Const::Vec<T, 2>{ left[0], left[Width - 1] }
	);

	testForResult(
		"Const::Vec swizzle (runtime)",
		left.swizzle(size_t(0), Width - 1),
		Const::Vec<T, 2>{ left[0], left[Width - 1] }
	);

	testByCheck(
		"Const::Vec operator <=>",
		[&]() { return std::is_eq(left <=> left); }
	);

	testForResult("Const::Vec size()", left.size(), Width);
}

void testVec()
{
	test_vec2<uint8_t>();
	test_vec2<uint16_t>();
	test_vec2<uint32_t>();
	test_vec2<uint64_t>();
	
	test_vec2<int8_t>();
	test_vec2<int16_t>();
	test_vec2<int32_t>();
	test_vec2<int64_t>();

	test_vec2<float32_t>();
	test_vec2<float64_t>();

	///////////////////////////////

	test_vec3<uint8_t>();
	test_vec3<uint16_t>();
	test_vec3<uint32_t>();
	test_vec3<uint64_t>();
	
	test_vec3<int8_t>();
	test_vec3<int16_t>();
	test_vec3<int32_t>();
	test_vec3<int64_t>();

	test_vec3<float32_t>();
	test_vec3<float64_t>();

	///////////////////////////////

	test_vec4<uint8_t>();
	test_vec4<uint16_t>();
	test_vec4<uint32_t>();
	test_vec4<uint64_t>();
	
	test_vec4<int8_t>();
	test_vec4<int16_t>();
	test_vec4<int32_t>();
	test_vec4<int64_t>();

	test_vec4<float32_t>();
	test_vec4<float64_t>();

	///////////////////////////////

	test_vec4<uint8_t>();
	test_vec4<uint16_t>();
	test_vec4<uint32_t>();
	test_vec4<uint64_t>();
	
	test_vec4<int8_t>();
	test_vec4<int16_t>();
	test_vec4<int32_t>();
	test_vec4<int64_t>();

	test_vec4<float32_t>();
	test_vec4<float64_t>();

	///////////////////////////////

	test_const_vec<uint8_t, 2>();
	test_const_vec<uint16_t, 2>();
	test_const_vec<uint32_t, 2>();
	test_const_vec<uint64_t, 2>();
	
	test_const_vec<int8_t, 2>();
	test_const_vec<int16_t, 2>();
	test_const_vec<int32_t, 2>();
	test_const_vec<int64_t, 2>();

	test_const_vec<float32_t, 2>();
	test_const_vec<float64_t, 2>();

	///////////////////////////////

	test_const_vec<uint8_t,  3>();
	test_const_vec<uint16_t, 3>();
	test_const_vec<uint32_t, 3>();
	test_const_vec<uint64_t, 3>();
	
	test_const_vec<int8_t,  3>();
	test_const_vec<int16_t, 3>();
	test_const_vec<int32_t, 3>();
	test_const_vec<int64_t, 3>();

	test_const_vec<float32_t, 3>();
	test_const_vec<float64_t, 3>();

	///////////////////////////////

	test_const_vec_mixed<int32_t,   float32_t, 2>();
	test_const_vec_mixed<int32_t,   float64_t, 2>();
	test_const_vec_mixed<float32_t, float64_t, 2>();
	test_const_vec_mixed<int8_t,    int32_t,   2>();
	test_const_vec_mixed<int32_t,   int64_t,   2>();
	test_const_vec_mixed<uint16_t,  uint64_t,  2>();

	///////////////////////////////

	test_const_vec_mixed<int32_t,   float32_t, 3>();
	test_const_vec_mixed<int32_t,   float64_t, 3>();
	test_const_vec_mixed<float32_t, float64_t, 3>();
	test_const_vec_mixed<int8_t,    int32_t,   3>();
	test_const_vec_mixed<int32_t,   int64_t,   3>();
	test_const_vec_mixed<uint16_t,  uint64_t,  3>();
}