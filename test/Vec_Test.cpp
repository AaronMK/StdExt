#include <StdExt/Vec.h>
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

	while( (T)0 == ret )
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

	testForResult<int>(
		"Vec2 comparison ( < )",
		0, (left < right).compare( Vec2<num_t>(left[0] < right[0], left[1] < right[1]))
	);

	testForResult<int>(
		"Vec2 comparison ( <= )",
		0, (left <= right).compare( Vec2<num_t>(left[0] <= right[0], left[1] <= right[1]))
	);

	testForResult<int>(
		"Vec2 comparison ( == )",
		0, (left == right).compare( Vec2<num_t>(left[0] == right[0], left[1] == right[1]))
	);

	testForResult<int>(
		"Vec comparison ( != )",
		0, (left != right).compare( Vec2<num_t>(left[0] != right[0], left[1] != right[1]))
	);

	testForResult<int>(
		"Vec2 comparison ( >= )",
		0, (left >= right).compare( Vec2<num_t>(left[0] >= right[0], left[1] >= right[1]))
	);

	testForResult<int>(
		"Vec2 comparison ( > )",
		0, (left > right).compare( Vec2<num_t>(left[0] > right[0], left[1] > right[1]))
	);

	testForResult<int>(
		"Vec2 operator ( + )",
		0, (left + right).compare( Vec2<num_t>(left[0] + right[0], left[1] + right[1]))
	);

	testForResult<int>(
		"Vec2 operator ( - )",
		0, (left - right).compare( Vec2<num_t>(left[0] - right[0], left[1] - right[1]))
	);

	testForResult<int>(
		"Vec2 operator ( * )",
		0, (left * right).compare( Vec2<num_t>(left[0] * right[0], left[1] * right[1]))
	);

	testForResult<int>(
		"Vec2 operator ( / )",
		0, (left / right).compare( Vec2<num_t>(left[0] / right[0], left[1] / right[1]))
	);

	Vec2<num_t> modified = left;
	modified += right;

	testForResult<int>(
		"Vec2 operator ( += )",
		0, (left + right).compare(modified)
	);

	modified = left;
	modified -= right;

	testForResult<int>(
		"Vec2 operator ( -= )",
		0, (left - right).compare(modified)
	);

	modified = left;
	modified *= right;

	testForResult<int>(
		"Vec2 operator ( *= )",
		0, (left * right).compare(modified)
	);

	modified = left;
	modified /= right;

	testForResult<int>(
		"Vec2 operator ( /= )",
		0, (left / right).compare(modified)
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

	testForResult<int>(
		"Vec3 comparison ( < )",
		0, (left < right).compare( Vec3<num_t>(left[0] < right[0], left[1] < right[1], left[2] < right[2]))
	);

	testForResult<int>(
		"Vec3 comparison ( <= )",
		0, (left <= right).compare( Vec3<num_t>(left[0] <= right[0], left[1] <= right[1], left[2] <= right[2]))
	);

	testForResult<int>(
		"Vec3 comparison ( == )",
		0, (left == right).compare( Vec3<num_t>(left[0] == right[0], left[1] == right[1], left[2] == right[2]))
	);

	testForResult<int>(
		"Vec comparison ( != )",
		0, (left != right).compare( Vec3<num_t>(left[0] != right[0], left[1] != right[1], left[2] != right[2]))
	);

	testForResult<int>(
		"Vec3 comparison ( >= )",
		0, (left >= right).compare( Vec3<num_t>(left[0] >= right[0], left[1] >= right[1], left[2] >= right[2]))
	);

	testForResult<int>(
		"Vec3 comparison ( > )",
		0, (left > right).compare( Vec3<num_t>(left[0] > right[0], left[1] > right[1], left[2] > right[2]))
	);

	testForResult<int>(
		"Vec3 operator ( + )",
		0, (left + right).compare( Vec3<num_t>(left[0] + right[0], left[1] + right[1], left[2] + right[2]))
	);

	testForResult<int>(
		"Vec3 operator ( - )",
		0, (left - right).compare( Vec3<num_t>(left[0] - right[0], left[1] - right[1], left[2] - right[2]))
	);

	testForResult<int>(
		"Vec3 operator ( * )",
		0, (left * right).compare( Vec3<num_t>(left[0] * right[0], left[1] * right[1], left[2] * right[2]))
	);

	testForResult<int>(
		"Vec3 operator ( / )",
		0, (left / right).compare( Vec3<num_t>(left[0] / right[0], left[1] / right[1], left[2] / right[2]))
	);

	Vec3<num_t> modified = left;
	modified += right;

	testForResult<int>(
		"Vec3 operator ( += )",
		0, (left + right).compare(modified)
	);

	modified = left;
	modified -= right;

	testForResult<int>(
		"Vec3 operator ( -= )",
		0, (left - right).compare(modified)
	);

	modified = left;
	modified *= right;

	testForResult<int>(
		"Vec3 operator ( *= )",
		0, (left * right).compare(modified)
	);

	modified = left;
	modified /= right;

	testForResult<int>(
		"Vec3 operator ( /= )",
		0, (left / right).compare(modified)
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

	testForResult<int>(
		"Vec4 comparison ( < )",
		0, (left < right).compare( Vec4<num_t>(left[0] < right[0], left[1] < right[1], left[2] < right[2], left[3] < right[3]))
	);

	testForResult<int>(
		"Vec4 comparison ( <= )",
		0, (left <= right).compare( Vec4<num_t>(left[0] <= right[0], left[1] <= right[1], left[2] <= right[2], left[3] <= right[3]))
	);

	testForResult<int>(
		"Vec4 comparison ( == )",
		0, (left == right).compare( Vec4<num_t>(left[0] == right[0], left[1] == right[1], left[2] == right[2], left[3] == right[3]))
	);

	testForResult<int>(
		"Vec comparison ( != )",
		0, (left != right).compare( Vec4<num_t>(left[0] != right[0], left[1] != right[1], left[2] != right[2], left[3] != right[3]))
	);

	testForResult<int>(
		"Vec4 comparison ( >= )",
		0, (left >= right).compare( Vec4<num_t>(left[0] >= right[0], left[1] >= right[1], left[2] >= right[2], left[3] >= right[3]))
	);

	testForResult<int>(
		"Vec4 comparison ( > )",
		0, (left > right).compare( Vec4<num_t>(left[0] > right[0], left[1] > right[1], left[2] > right[2], left[3] > right[3]))
	);

	testForResult<int>(
		"Vec4 operator ( + )",
		0, (left + right).compare( Vec4<num_t>(left[0] + right[0], left[1] + right[1], left[2] + right[2], left[3] + right[3]))
	);

	testForResult<int>(
		"Vec4 operator ( - )",
		0, (left - right).compare( Vec4<num_t>(left[0] - right[0], left[1] - right[1], left[2] - right[2], left[3] - right[3]))
	);

	testForResult<int>(
		"Vec4 operator ( * )",
		0, (left * right).compare( Vec4<num_t>(left[0] * right[0], left[1] * right[1], left[2] * right[2], left[3] * right[3]))
	);

	testForResult<int>(
		"Vec4 operator ( / )",
		0, (left / right).compare( Vec4<num_t>(left[0] / right[0], left[1] / right[1], left[2] / right[2], left[3] / right[3]))
	);

	Vec4<num_t> modified = left;
	modified += right;

	testForResult<int>(
		"Vec4 operator ( += )",
		0, (left + right).compare(modified)
	);

	modified = left;
	modified -= right;

	testForResult<int>(
		"Vec4 operator ( -= )",
		0, (left - right).compare(modified)
	);

	modified = left;
	modified *= right;

	testForResult<int>(
		"Vec4 operator ( *= )",
		0, (left * right).compare(modified)
	);

	modified = left;
	modified /= right;

	testForResult<int>(
		"Vec4 operator ( /= )",
		0, (left / right).compare(modified)
	);

	testForResult<int>(
		"Shuffle using mask 1",
		0, shuffle<0, 2, 1, 3>(left, right).compare( Vec4<num_t>(left[0], left[2], right[1], right[3]) )
	);

	testForResult<int>(
		"Shuffle using mask 2",
		0, shuffle<3, 1, 2, 0>(left, right).compare( Vec4<num_t>(left[3], left[1], right[2], right[0]) )
	);

	testBinarySerialize(r());
	testXmlSerialize(r());
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
}