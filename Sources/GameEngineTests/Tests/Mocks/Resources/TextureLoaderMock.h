#pragma once
#include <gmock/gmock.h>
#include "../Api/GraphicsApiMock.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "OpenGLLoaderMock.h"
#include "Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
class TextureLoaderMock : public ITextureLoader
{
public:
    MOCK_METHOD4(CreateTexture, Texture*(const std::string&, const TextureParameters&, const TextureSize&, RawData));
    MOCK_METHOD2(LoadTexture, Texture*(const InputFileName&, const TextureParameters&));
    MOCK_METHOD2(LoadCubeMap, Texture*(const std::array<InputFileName, 6>&, const TextureParameters&));
    MOCK_METHOD2(LoadHeightMap, Texture*(const InputFileName&, const TextureParameters&));
    MOCK_METHOD2(CreateNormalMap, Texture*(const HeightMap&, const vec3&));
    MOCK_METHOD0(GetGraphicsApi, GraphicsApi::IGraphicsApi&());
    MOCK_CONST_METHOD5(SaveTextureToFile, void(const OutputFileName&, const std::vector<uint8>&, const TextureSize&,
                                               uint8, GraphicsApi::TextureFormat));
    MOCK_METHOD1(DeleteTexture, void(Texture&));
};
}  // namespace GameEngine
