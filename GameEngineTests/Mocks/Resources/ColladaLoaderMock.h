#pragma once
#include "TextureLoaderMock.h"
#include "Resources/Models/WBLoader/Collada/ColladaDae.h"
#include <gtest/gtest.h>

struct ColladaLoaderMock : public WBLoader::ColladaDae, public ::testing::Test
{
	ColladaLoaderMock() : WBLoader::ColladaDae(textureLoaderMock)
	{
	}

	WBLoader::ColladaTypes::ArrayType GetType(const std::string& str)
	{
		return WBLoader::ColladaDae::GetArrayType(str);
	}

	std::vector<float> GetFloats(const std::string& str)
	{
		return GetFloatsFromString(str);
	}

	TextureLoaderMock textureLoaderMock;
};