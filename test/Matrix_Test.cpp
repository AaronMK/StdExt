#include <StdExt/Matrix.h>
#include <StdExt/Utility.h>

#include <StdExt/Streams/BufferedStream.h>
#include <StdExt/Test/Test.h>

#include "SerializeTesting.h"

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::Streams;
using namespace StdExt::Serialize::XML;
using namespace StdExt::Serialize::Binary;

void testMatrix()
{
	{
		Matrix2x2<float64_t> matrix_left( 5.5f, -6.3f,
		                                  2.5f, 17.3f );

		Matrix2x2<float64_t> matrix_right( 12.5f, 13.3f,
		                                   19.5f, -7.3f );

		Matrix2x2<float64_t> expected = Matrix2x2<float64_t>( -54.1f,   119.14f,
		                                                       368.6f, -93.04f );

		testForResult< Matrix2x2<float64_t> >(
			"Matrix2x2 * Matrix2x2",
			expected, matrix_left * matrix_right
		);

		testForResult< Vec2<float64_t> >(
			"Matrix2x2 * Vec2",
			matrix_left * Vec2<float64_t>( 12.5f, 19.5f), Vec2<float64_t>(-54.1f, 368.6f)
		);

		expected = Matrix2x2<float64_t>( 11.0f, -12.6f,
		                                 5.0f,   34.6f );

		testForResult< Matrix2x2<float64_t> >(
			"Matrix2x2 * float",
			expected, matrix_left * 2.0f
		);
		
		testForResult< Matrix2x2<float64_t> >(
			"Matrix2x2 / float",
			expected, matrix_left / 0.5f
		);

		expected = Matrix2x2<float64_t>( 18.0f,  7.0f,
		                                 22.0f,  10.0f );

		testForResult< Matrix2x2<float64_t> >(
			"Matrix2x2 + Matrix2x2",
			expected, matrix_left + matrix_right
		);

		expected = Matrix2x2<float64_t>( -7.0f,  -19.6f,
		                                 -17.0f,  24.6f );

		testForResult< Matrix2x2<float64_t> >(
			"Matrix2x2 - Matrix2x2",
			expected, matrix_left - matrix_right
		);

		testForResult< float64_t >(
			"Matrix2x2 determinant()",
			110.9f, matrix_left.determinant()
		);

		expected = Matrix2x2<float64_t> ( 0.155996f, 0.0568079f,
		                                 -0.022542f, 0.049594f );

		testForResult< Matrix2x2<float64_t> >(
			"Matrix2x2 inverse()",
			expected, matrix_left.inverse()
		);

		testForResult< Vec2<float64_t> >(
			"Matrix2x2 * Vec2",
			matrix_left * Vec2<float64_t>(8.0f, 3.0f), Vec2<float64_t>(25.1f, 71.9f)
		);

		testBinarySerialize(expected);
		testXmlSerialize(expected);
	}

	{
		Matrix3x3<float64_t> matrix_left( 5.5f, -6.3f,  5.6f,
		                                  2.5f, 17.3f, -3.5f,
		                                  1.2f, 0.35f,  18.2 );

		Matrix3x3<float64_t> matrix_right( 12.5f, 13.3f, 8.5f,
		                                   19.5f, -7.3f, 3.1f,
		                                   1.5f,   8.1f, 6.3f );

		testForResult< Vec3<float64_t> >(
			"Matrix3x3 * Vec3",
			matrix_left * Vec3<float64_t>( 12.5, 19.5, 1.5), Vec3<float64_t>(-45.7, 363.35, 49.125)
		);

		Matrix3x3<float64_t> expected = Matrix3x3<float64_t>( -45.7f,   164.5f,  62.5f,
		                                                       363.35, -121.39,  52.83,
		                                                       49.125,  160.825, 125.945 );

		testForResult< Matrix3x3<float64_t> >(
			"Matrix3x3 * Matrix3x3",
			expected, matrix_left * matrix_right
		);

		expected = Matrix3x3<float64_t>( 11,  -12.6,  11.2, 
		                                 5,    34.6, -7, 
		                                 2.4,  0.7,   36.4  );

		testForResult< Matrix3x3<float64_t> >(
			"Matrix3x3 * float",
			expected, matrix_left * 2.0f
		);
		
		testForResult< Matrix3x3<float64_t> >(
			"Matrix3x3 / float",
			expected, matrix_left / 0.5f
		);

		expected = Matrix3x3<float64_t>( 18,  7,     14.1, 
		                                 22,  10,   -0.4, 
		                                 2.7, 8.45,  24.5  );

		testForResult< Matrix3x3<float64_t> >(
			"Matrix3x3 + Matrix3x3",
			expected, matrix_left + matrix_right
		);

		expected = Matrix3x3<float64_t>( -7,   -19.6, -2.9, 
		                                 -17,   24.6, -6.6, 
		                                 -0.3, -7.75,  11.9  );

		testForResult< Matrix3x3<float64_t> >(
			"Matrix3x3 - Matrix3x3",
			expected, matrix_left - matrix_right
		);

		testForResult< float64_t >(
			"Matrix3x3 determinant()",
			1940.2215, matrix_left.determinant()
		);

		expected = Matrix3x3<float64_t> ( 0.162911811873026,   0.0601065393822303,  -0.0385677614643483, 
		                                 -0.025615632029642,   0.0481285255317499,   0.0171372186113802, 
		                                 -0.0102488298372119, -0.00488861709861477,  0.0571584223760019 );

		testForResult< Matrix3x3<float64_t> >(
			"Matrix3x3 inverse()",
			expected, matrix_left.inverse()
		);

		testBinarySerialize(expected);
		testXmlSerialize(expected);
	}

	{
		Matrix4x4<float64_t> matrix_left( 5.5, -6.3,  5.6,  0.8,
		                                  2.5, 17.3, -3.5,  3.2,
		                                  1.2, 0.35,  18.2, 7.9,
		                                  20,  6.1,   4.2,  1.1 );

		Matrix4x4<float64_t> matrix_right( 12.5,  13.3, 8.5, 6.7,
		                                   19.5, -7.3, 3.1,  5.3,
		                                   1.5,   8.1, 6.3,  8.7,
		                                   4.2,   6.9, 1.3,  6.2);

		testForResult< Vec4<float64_t> >(
			"Matrix4x4 * Vec4",
			matrix_left * Vec4<float64_t>( 12.5, 19.5, 1.5, 4.2), Vec4<float64_t>(-42.34, 376.79, 82.305, 379.87)
		);

		Matrix4x4<float64_t> expected = Matrix4x4<float64_t>( -42.34, 170.02,  63.54,   57.14,
		                                                      376.79, -99.31,  56.99,   97.83, 
		                                                      82.305, 215.335, 136.215, 217.215, 
		                                                      379.87, 263.08,  216.8,   209.69  );

		testForResult< Matrix4x4<float64_t> >(
			"Matrix4x4 * Matrix4x4",
			expected, matrix_left * matrix_right
		);

		expected = Matrix4x4<float64_t>( 11,  -12.6,  11.2, 1.6, 
		                                 5,    34.6, -7,    6.4,
		                                 2.4,  0.7,   36.4, 15.8,
		                                 40,   12.2,  8.4,  2.2 );

		testForResult< Matrix4x4<float64_t> >(
			"Matrix4x4 * float",
			expected, matrix_left * 2.0f
		);
		
		testForResult< Matrix4x4<float64_t> >(
			"Matrix4x4 / float",
			expected, matrix_left / 0.5f
		);

		expected = Matrix4x4<float64_t>( 18,   7,     14.1, 7.5,
		                                 22,   10,   -0.4,  8.5,
		                                 2.7,  8.45,  24.5, 16.6,
		                                 24.2, 13,    5.5,  7.3 );

		testForResult< Matrix4x4<float64_t> >(
			"Matrix4x4 + Matrix4x4",
			expected, matrix_left + matrix_right
		);

		expected = Matrix4x4<float64_t>( -7, -19.6, -2.9, -5.9, 
		                                 -17, 24.6, -6.6, -2.1, 
		                                 -0.3, -7.75, 11.9, -0.8, 
		                                  15.8, -0.8, 2.9, -5.1  );

		testForResult< Matrix4x4<float64_t> >(
			"Matrix4x4 - Matrix4x4",
			expected, matrix_left - matrix_right
		);

		expected = Matrix4x4<float64_t> (  0.14250852005103,   0.0502669797737892, -0.0357212936281104, 0.00667006213192983, 
		                                  -0.330674518563011, -0.0989872009067538,  0.0596960525608924, 0.0997271295645186, 
		                                  -0.49546653204336,  -0.238886569271271,   0.124851253265872,  0.158623042274881, 
		                                   1.13445781526898,   0.547096291813412,  -0.158268465250816, -0.370866827760598 );

		testForResult< Matrix4x4<float64_t> >(
			"Matrix4x4 inverse()",
			expected, matrix_left.inverse()
		);

		testForResult< float64_t >(
			"Matrix4x4 determinant()",
			-5231.585, matrix_left.determinant()
		);

		testBinarySerialize(expected);
		testXmlSerialize(expected);
	}
}