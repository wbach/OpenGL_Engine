#ifndef COLLADALOADERMOCK_H
#define COLLADALOADERMOCK_H
#include <gtest/gtest.h>
#include "Resources/Models/WBLoader/Collada/ColladaDae.h"
#include "TextureLoaderMock.h"

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

    std::string GetObjectName(const std::string& str) const
    {
        return GetName(str, "-mesh");
    }

    TextureLoaderMock textureLoaderMock;
};
#endif // COLLADALOADERMOCK_H
