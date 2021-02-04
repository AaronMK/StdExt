#include <StdExt/Vec.h>

#include <StdExt/Test/Test.h>

using namespace StdExt;
using namespace StdExt::Test;

int main()
{
	Vec2<int> v2i_1(1, 2);
	Vec2<int> v2i_2(5, 2);

	Vec2<float> v2f_1(1.0f, 2.0f);
	Vec2<float> v2f_2(5.0f, 2.0f);

	testForResult<int>(
		"Vec2<int> comparison ( < )",
		0, (v2i_1 < v2i_2).compare( Vec2<bool>(true, false))
	);

	testForResult<int>(
		"Vec2<int> comparison ( <= )",
		0, (v2i_1 <= v2i_2).compare( Vec2<bool>(true, true))
	);

	testForResult<int>(
		"Vec2<int> comparison ( == )",
		0, (v2i_1 == v2i_2).compare( Vec2<bool>(false, true))
	);

	testForResult<int>(
		"Vec2<int> comparison ( != )",
		0, (v2i_1 != v2i_2).compare( Vec2<bool>(true, false))
	);

	testForResult<int>(
		"Vec2<int> comparison ( >= )",
		0, (v2i_1 >= v2i_2).compare( Vec2<bool>(false, true))
	);

	testForResult<int>(
		"Vec2<int> comparison ( > )",
		0, (v2i_1 > v2i_2).compare( Vec2<bool>(false, false))
	);

	testForResult<int>(
		"Vec2<int> operator ( + )",
		0, (v2i_1 + v2i_2).compare( Vec2<int>(6, 4))
	);

	testForResult<int>(
		"Vec2<int> operator ( - )",
		0, (v2i_1 - v2i_2).compare( Vec2<int>(-4, 0))
	);

	testForResult<int>(
		"Vec2<int> operator ( / )",
		0, (v2i_1 / v2i_2).compare( Vec2<int>(1/5, 1))
	);

	testForResult<int>(
		"Vec2<float> operator ( / )",
		0, (v2f_1 / v2f_2).compare( Vec2<float>(0.2f, 1.0f))
	);

	testForResult<int>(
		"Vec2<float> operator ( / float )",
		0, (v2f_1 / 5.0f).compare( Vec2<float>(0.2f, 0.4f))
	);

	testForResult<int>(
		"Vec2<int> operator ( * )",
		0, (v2i_1 * v2i_2).compare( Vec2<int>(5, 4))
	);

	testForResult<int>(
		"Vec2<float> operator ( * float )",
		0, (v2f_1 * 5.0f).compare( Vec2<float>(5.0f, 10.0f))
	);

	v2i_1 += Vec2<int>(5, 8);

	testForResult<int>(
		"Vec2<int> operator ( += )",
		0, v2i_1.compare( Vec2<int>(6, 10))
	);

	v2i_1 -= Vec2<int>(-5, 3);

	testForResult<int>(
		"Vec2<int> operator ( -= )",
		0, v2i_1.compare( Vec2<int>(11, 7))
	);

	v2i_1 *= Vec2<int>(3, 5);

	testForResult<int>(
		"Vec2<int> operator ( *= )",
		0, v2i_1.compare( Vec2<int>(33, 35))
	);

	v2i_1 *= 2;

	testForResult<int>(
		"Vec2<int> operator ( *= int )",
		0, v2i_1.compare( Vec2<int>(66, 70))
	);

	v2i_1 /= Vec2<int>(33, 7);

	testForResult<int>(
		"Vec2<int> operator ( /= )",
		0, v2i_1.compare( Vec2<int>(2, 10))
	);

	v2i_1 /= 2;

	testForResult<int>(
		"Vec2<int> operator ( /= int )",
		0, v2i_1.compare( Vec2<int>(1, 5))
	);

	Vec2<float> v2f_test(36.0f, 72.0f);
	v2f_test /= 6.0f;

	testForResult<int>(
		"Vec2<int> operator ( /= float )",
		0, v2f_test.compare( Vec2<float>(6.0f, 12.0f))
	);

	//////////////////////////////////

	Vec3<int> v3i_1(0, 2, 7);
	Vec3<int> v3i_2(3, 2, 4);

	Vec3<float> v3f_1(1.0f, 2.0f, 7.0f);
	Vec3<float> v3f_2(5.0f, 2.0f, 4.0f);

	testForResult<int>(
		"Vec3<int> comparison ( < )",
		0, (v3i_1 < v3i_2).compare( Vec3<bool>(true, false, false))
	);

	testForResult<int>(
		"Vec3<int> comparison ( <= )",
		0, (v3i_1 <= v3i_2).compare( Vec3<bool>(true, true, false))
	);

	testForResult<int>(
		"Vec3<int> comparison ( == )",
		0, (v3i_1 == v3i_2).compare( Vec3<bool>(false, true, false))
	);

	testForResult<int>(
		"Vec3<int> comparison ( != )",
		0, (v3i_1 != v3i_2).compare( Vec3<bool>(true, false, true))
	);

	testForResult<int>(
		"Vec3<int> comparison ( >= )",
		0, (v3i_1 >= v3i_2).compare( Vec3<bool>(false, true, true))
	);

	testForResult<int>(
		"Vec3<int> comparison ( > )",
		0, (v3i_1 > v3i_2).compare( Vec3<bool>(false, false, true))
	);

	testForResult<int>(
		"Vec3<int> operator ( + )",
		0, (v3i_1 + v3i_2).compare( Vec3<int>(3, 4, 11))
	);

	testForResult<int>(
		"Vec3<int> operator ( - )",
		0, (v3i_1 - v3i_2).compare( Vec3<int>(-3, 0, 3))
	);

	testForResult<int>(
		"Vec3<int> operator ( / )",
		0, (v3i_1 / v3i_2).compare( Vec3<int>(0, 1, 1))
	);

	testForResult<int>(
		"Vec3<float> operator ( / )",
		0, (v3f_1 / v3f_2).compare( Vec3<float>(0.2f, 1.0f, 1.75f))
	);

	testForResult<int>(
		"Vec3<float> operator ( / float )",
		0, (v3f_1 / 5.0f).compare( Vec3<float>(0.2f, 0.4f, 1.4f))
	);

	testForResult<int>(
		"Vec3<int> operator ( * )",
		0, (v3i_1 * v3i_2).compare( Vec3<int>(0, 4, 28))
	);

	testForResult<int>(
		"Vec3<float> operator ( * float )",
		0, (v3f_1 * 5.0f).compare( Vec3<float>(5.0f, 10.0f, 35.0f))
	);

	v3i_1 += Vec3<int>(8, 8, -5);

	testForResult<int>(
		"Vec3<int> operator ( += )",
		0, v3i_1.compare( Vec3<int>(8, 10, 2))
	);

	v3i_1 -= Vec3<int>(-5, 3, -3);

	testForResult<int>(
		"Vec3<int> operator ( -= )",
		0, v3i_1.compare( Vec3<int>(13, 7, 5))
	);

	v3i_1 *= Vec3<int>(3, 5, 2);

	testForResult<int>(
		"Vec3<int> operator ( *= )",
		0, v3i_1.compare( Vec3<int>(39, 35, 10))
	);

	v3i_1 *= 2;

	testForResult<int>(
		"Vec3<int> operator ( *= int )",
		0, v3i_1.compare( Vec3<int>(78, 70, 20))
	);

	v3i_1 /= Vec3<int>(2, 7, 5);

	testForResult<int>(
		"Vec3<int> operator ( /= )",
		0, v3i_1.compare( Vec3<int>(39, 10, 4))
	);

	v3i_1 /= 2;

	testForResult<int>(
		"Vec3<int> operator ( /= int )",
		0, v3i_1.compare( Vec3<int>(19, 5, 2))
	);

	Vec3<float> v3f_test(36.0f, 72.0f, 12.0f);
	v3f_test /= 6.0f;

	testForResult<int>(
		"Vec3<int> operator ( /= float )",
		0, v3f_test.compare( Vec3<float>(6.0f, 12.0f, 2.0f))
	);

	//////////////////////////////////

	Vec4<int> v4i_1(0, 2, 7, -3);
	Vec4<int> v4i_2(3, 2, 4, 10);

	Vec4<float> v4f_1(1.0f, 2.0f, 7.0f, -3.0f);
	Vec4<float> v4f_2(5.0f, 2.0f, 4.0f, 8.0f);

	testForResult<int>(
		"Vec4<int> comparison ( < )",
		0, (v4i_1 < v4i_2).compare( Vec4<bool>(true, false, false, true))
	);

	testForResult<int>(
		"Vec4<int> comparison ( <= )",
		0, (v4i_1 <= v4i_2).compare( Vec4<bool>(true, true, false, true))
	);

	testForResult<int>(
		"Vec4<int> comparison ( == )",
		0, (v4i_1 == v4i_2).compare( Vec4<bool>(false, true, false, false))
	);

	testForResult<int>(
		"Vec4<int> comparison ( != )",
		0, (v4i_1 != v4i_2).compare( Vec4<bool>(true, false, true, true))
	);

	testForResult<int>(
		"Vec4<int> comparison ( >= )",
		0, (v4i_1 >= v4i_2).compare( Vec4<bool>(false, true, true, false))
	);

	testForResult<int>(
		"Vec4<int> comparison ( > )",
		0, (v4i_1 > v4i_2).compare( Vec4<bool>(false, false, true, false))
	);

	testForResult<int>(
		"Vec4<int> operator ( + )",
		0, (v4i_1 + v4i_2).compare( Vec4<int>(3, 4, 11, 7))
	);

	testForResult<int>(
		"Vec4<int> operator ( - )",
		0, (v4i_1 - v4i_2).compare( Vec4<int>(-3, 0, 3, -13))
	);

	testForResult<int>(
		"Vec4<int> operator ( / )",
		0, (v4i_1 / v4i_2).compare( Vec4<int>(0, 1, 1, -3/10))
	);

	testForResult<int>(
		"Vec4<float> operator ( / )",
		0, (v4f_1 / v4f_2).compare( Vec4<float>(0.2f, 1.0f, 1.75f, -3.0f/8.0f))
	);

	testForResult<int>(
		"Vec4<float> operator ( / float )",
		0, (v4f_1 / 5.0f).compare( Vec4<float>(0.2f, 0.4f, 1.4f, -0.6f))
	);

	testForResult<int>(
		"Vec4<int> operator ( * )",
		0, (v4i_1 * v4i_2).compare( Vec4<int>(0, 4, 28, -30))
	);

	testForResult<int>(
		"Vec4<float> operator ( * float )",
		0, (v4f_1 * 5.0f).compare( Vec4<float>(5.0f, 10.0f, 35.0f, -15))
	);

	v4i_1 += Vec4<int>(8, 8, -2, 18);

	testForResult<int>(
		"Vec4<int> operator ( += )",
		0, v4i_1.compare( Vec4<int>(8, 10, 5, 15))
	);

	v4i_1 -= Vec4<int>(-5, 3, -3, 5);

	testForResult<int>(
		"Vec4<int> operator ( -= )",
		0, v4i_1.compare( Vec4<int>(13, 7, 8, 10))
	);

	v4i_1 *= Vec4<int>(3, 5, 2, 4);

	testForResult<int>(
		"Vec4<int> operator ( *= )",
		0, v4i_1.compare( Vec4<int>(39, 35, 16, 40))
	);

	v4i_1 *= 2;

	testForResult<int>(
		"Vec4<int> operator ( *= int )",
		0, v4i_1.compare( Vec4<int>(78, 70, 32, 80))
	);

	v4i_1 /= Vec4<int>(2, 7, 5, 10);

	testForResult<int>(
		"Vec4<int> operator ( /= )",
		0, v4i_1.compare( Vec4<int>(39, 10, 6, 8))
	);

	v4i_1 /= 2;

	testForResult<int>(
		"Vec4<int> operator ( /= int )",
		0, v4i_1.compare( Vec4<int>(19, 5, 3, 4))
	);

	Vec4<float> v4f_test(36.0f, 72.0f, 12.0f, 24.0f);
	v4f_test /= 6.0f;

	testForResult<int>(
		"Vec4<int> operator ( /= float )",
		0, v4f_test.compare( Vec4<float>(6.0f, 12.0f, 2.0f, 4.0f))
	);

	return 0;
}