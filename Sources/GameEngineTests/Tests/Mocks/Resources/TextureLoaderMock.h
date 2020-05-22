#pragma once
#include <gmock/gmock.h>
#include "../Api/GraphicsApiMock.h"
#include "OpenGLLoaderMock.h"
#include "Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
class TextureLoaderMock : public ITextureLoader
{
 public:
  MOCK_METHOD3(CreateTexture,
      GeneralTexture*(const std::string&, const TextureParameters&, const GraphicsApi::Image&));
  MOCK_METHOD1(UpdateTexture,
      void(const GeneralTexture&));
  MOCK_METHOD2(UpdateTexture,
      void(const GeneralTexture&, const std::string&));
  MOCK_METHOD2(LoadTexture,
      GeneralTexture*(const File&, const TextureParameters&));
  MOCK_METHOD2(LoadCubeMap,
      CubeMapTexture*(const std::array<File, 6>&, const TextureParameters&));
  MOCK_METHOD2(LoadHeightMap,
      HeightMap*(const File&, const TextureParameters&));
  MOCK_METHOD2(CreateNormalMap,
      GeneralTexture*(const HeightMap&, const vec3&));
  MOCK_METHOD0(GetGraphicsApi,
      GraphicsApi::IGraphicsApi&());
  MOCK_METHOD1(DeleteTexture,
      void(Texture&));
};
}  // namespace GameEngine
