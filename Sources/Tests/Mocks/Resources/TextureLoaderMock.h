#pragma once
#include <GameEngine/Resources/ITextureLoader.h>
#include <gmock/gmock.h>

#include <vector>

#include "../Api/GraphicsApiMock.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "Image/Image.h"
#include "OpenGLLoaderMock.h"

namespace GameEngine
{
class TextureLoaderMock : public ITextureLoader
{
public:
    MOCK_METHOD(GeneralTexture*, CreateTexture, (const std::string&, const TextureParameters&, Utils::Image&&), (override));
    MOCK_METHOD(ArrayTexture*, CreateTexture, (const std::string&, const TextureParameters&, std::vector<Utils::Image>&&),
                (override));

    MOCK_METHOD(void, UpdateTexture, (GeneralTexture&), (override));
    MOCK_METHOD(void, UpdateTexture, (GeneralTexture*&, const std::string&), (override));

    MOCK_METHOD(GeneralTexture*, LoadTexture, (const std::string&, const unsigned char*, unsigned int, const TextureParameters&),
                (override));
    MOCK_METHOD(GeneralTexture*, LoadTexture, (const std::string&, Utils::Image&&, const TextureParameters&), (override));
    MOCK_METHOD(GeneralTexture*, LoadTexture, (const File&, const TextureParameters&), (override));

    MOCK_METHOD(CubeMapTexture*, LoadCubeMap, ((const std::array<File, 6>&), const TextureParameters&), (override));
    MOCK_METHOD(HeightMap*, LoadHeightMap, (const File&, const TextureParameters&), (override));
    MOCK_METHOD(HeightMap*, CreateHeightMap, (const File&, const vec2ui&, const TextureParameters&), (override));
    MOCK_METHOD(GeneralTexture*, CreateNormalMap, (const HeightMap&, const vec3&), (override));

    MOCK_METHOD(GraphicsApi::IGraphicsApi&, GetGraphicsApi, (), (override));
    MOCK_METHOD(void, DeleteTexture, (Texture&), (override));

    MOCK_METHOD(void, LockReleaseResources, (), (override));
    MOCK_METHOD(void, UnlockReleaseResources, (), (override));
};
}  // namespace GameEngine
