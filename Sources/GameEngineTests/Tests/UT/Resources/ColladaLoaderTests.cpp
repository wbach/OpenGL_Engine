#include "../../Mocks/Resources/ColladaLoaderMock.h"
#include "Engine/Configuration.h"
#include <gtest/gtest.h>

TEST_F(ColladaLoaderMock, GetFloatsFromString)
{
	std::cout << "aaa12" << std::endl;
	std::vector<float> ref( { 1.f, 1.f, -1.f, 1.f, -1.f, -1.f, -1.f, -0.9999998f, -1.f, -0.9999997f, 1.f, -1.f, 1.f, 0.9999995f, 1.f, 0.9999994f, -1.000001f, 1.f, -1.f, -0.9999997f
		, 1.f, -1.f, 1.f, 1.f });
	std::cout << "aaa11" << std::endl;
	std::string str = "1 1 -1 1 -1 -1 -1 -0.9999998 -1 -0.9999997 1 -1 1 0.9999995 1 0.9999994 -1.000001 1 -1 -0.9999997 1 -1 1 1";
	std::cout << "aaa1" << std::endl;
	auto result = GameEngine::Collada::GetFloatsFromString(str);
	std::cout << "aaa" << std::endl;
	ASSERT_EQ(result.size(), ref.size());

	return;
	for (uint32 x = 0; x < ref.size(); ++x)
	{
		//std::cout << ref[x] << " <==> " << result[x] << std::endl;
		EXPECT_EQ(ref[x], result[x]);
	}
}

//TEST_F(ColladaLoaderMock, GetGeometryNameFromString)
//{
//    WBLoader::ColladaDae model;
//}
