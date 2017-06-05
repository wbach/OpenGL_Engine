#pragma once
#include "TextureLoaderMock.h"
#include "Resources/Models/WBLoader/ColladaDae.h"
#include <gtest/gtest.h>

struct ColladaLoaderMock : public WBLoader::ColladaDae, public ::testing::Test
{
	ColladaLoaderMock() : WBLoader::ColladaDae(textureLoaderMock)
	{
	}

	WBLoader::ColladaDae::ArrayType GetType(const std::string& str)
	{
		return WBLoader::ColladaDae::GetArrayType(str);
	}

	TextureLoaderMock textureLoaderMock;
};