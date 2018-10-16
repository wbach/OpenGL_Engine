#include "../../Mocks/Resources/ColladaLoaderMock.h"
#include "Engine/Configuration.h"
#include <gtest/gtest.h>

struct ColladaLoaderShould : public ::testing::Test
{
	ColladaLoaderShould()
	{
		sut_.reset(new ColladaLoaderMock(textureLoaderMock));
	}
	TextureLoaderMock textureLoaderMock;
	std::unique_ptr<ColladaLoaderMock> sut_;
};

TEST_F(ColladaLoaderShould, GetFloatsFromString)
{
	std::vector<float> ref( { 1.f, 1.f, -1.f, 1.f, -1.f, -1.f, -1.f, -0.9999998f, -1.f, -0.9999997f, 1.f, -1.f, 1.f, 0.9999995f, 1.f, 0.9999994f, -1.000001f, 1.f, -1.f, -0.9999997f
		, 1.f, -1.f, 1.f, 1.f });

	std::string str = "1 1 -1 1 -1 -1 -1 -0.9999998 -1 -0.9999997 1 -1 1 0.9999995 1 0.9999994 -1.000001 1 -1 -0.9999997 1 -1 1 1";

	auto result = GameEngine::Collada::GetFloatsFromString(str);

	ASSERT_EQ(result.size(), ref.size());

	for (uint32 x = 0; x < ref.size(); ++x)
	{
		EXPECT_EQ(ref[x], result[x]);
	}
}
