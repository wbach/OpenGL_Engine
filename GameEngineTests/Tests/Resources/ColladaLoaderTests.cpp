#include "../../Mocks/Resources/ColladaLoaderMock.h"
#include "Resources/Models/WBLoader/Collada/ColladaDaeTypes.h"
#include <gtest/gtest.h>

TEST_F(ColladaLoaderMock, GetTypeFromStringTest)
{
	const std::string position_str = "Cube-mesh-positions";
	EXPECT_EQ(WBLoader::ColladaTypes::POSITIONS, GetType(position_str));

	const std::string normals_str = "Cube-mesh-normals";
	EXPECT_EQ(WBLoader::ColladaTypes::NORMALS, GetType(normals_str));

	const std::string text_coord = "Cube-mesh-map-0-array";
	EXPECT_EQ(WBLoader::ColladaTypes::TEXT_COORD, GetType(text_coord));

	EXPECT_NE(WBLoader::ColladaTypes::NORMALS, GetType(position_str));
	EXPECT_NE(WBLoader::ColladaTypes::POSITIONS, GetType(normals_str));
}

TEST_F(ColladaLoaderMock, GetFloatsFromString)
{
	std::vector<float> ref( { 1.f, 1.f, -1.f, 1.f, -1.f, -1.f, -1.f, -0.9999998f, -1.f, -0.9999997f, 1.f, -1.f, 1.f, 0.9999995f, 1.f, 0.9999994f, -1.000001f, 1.f, -1.f, -0.9999997f
		, 1.f, -1.f, 1.f, 1.f });

	std::string str = "1 1 -1 1 -1 -1 -1 -0.9999998 -1 -0.9999997 1 -1 1 0.9999995 1 0.9999994 -1.000001 1 -1 -0.9999997 1 -1 1 1";

	auto result = GetFloats(str);

	EXPECT_EQ(result.size(), ref.size());

	for (uint32 x = 0; x < ref.size(); ++x)
	{
		//std::cout << ref[x] << " <==> " << result[x] << std::endl;
		EXPECT_EQ(ref[x], result[x]);
	}
}
