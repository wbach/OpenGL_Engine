#ifndef COLLADALOADERMOCK_H
#define COLLADALOADERMOCK_H
#include <gtest/gtest.h>
#include "Resources/Models/WBLoader/Collada/Collada.h"
#include "TextureLoaderMock.h"

struct ColladaLoaderMock : public WBLoader::ColladaDae, public ::testing::Test
{
    ColladaLoaderMock() : WBLoader::ColladaDae(textureLoaderMock)
    {
    }
    TextureLoaderMock textureLoaderMock;
};
#endif // COLLADALOADERMOCK_H
