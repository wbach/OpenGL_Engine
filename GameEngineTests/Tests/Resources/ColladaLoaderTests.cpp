#include "../../Mocks/Resources/ColladaLoaderMock.h"
#include <gtest/gtest.h>

TEST_F(ColladaLoaderMock, GetTypeFromStringTest)
{
	const std::string position_str = "Cube-mesh-positions";
	EXPECT_EQ(ColladaLoaderMock::ArrayType::POSITIONS, GetType(position_str));

	const std::string normals_str = "Cube-mesh-normals";
	EXPECT_EQ(ColladaLoaderMock::ArrayType::NORMALS, GetType(normals_str));

	EXPECT_NE(ColladaLoaderMock::ArrayType::NORMALS, GetType(position_str));
	EXPECT_NE(ColladaLoaderMock::ArrayType::POSITIONS, GetType(normals_str));
}
