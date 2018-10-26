#ifndef COLLADALOADERMOCK_H
#define COLLADALOADERMOCK_H
#include "Resources/Models/WBLoader/Collada/Collada.h"
#include "TextureLoaderMock.h"

namespace GameEngine
{
struct ColladaLoaderMock : public WBLoader::ColladaDae
{
    ColladaLoaderMock(TextureLoaderMock& textureLoaderMock) : WBLoader::ColladaDae(textureLoaderMock)
    {
    }
   
};
} // namespace GameEngine
#endif // COLLADALOADERMOCK_H
