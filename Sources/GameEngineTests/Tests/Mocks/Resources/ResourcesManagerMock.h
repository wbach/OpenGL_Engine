#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/TextureLoader.h"
#include "Types.h"

namespace GameEngine
{
class ResourceManagerMock : public IResourceManager
{
public:
    MOCK_METHOD1(LoadModel, Model*(const std::string& file));
    MOCK_METHOD1(AddModel, void(Model* model));
    MOCK_METHOD1(GetModel, Model*(uint32 id));
    MOCK_METHOD0(GetGpuResourceLoader, IGpuResourceLoader&());
    MOCK_METHOD0(GetTextureLaoder, ITextureLoader&());
    MOCK_METHOD0(GetGraphicsApi, GraphicsApi::IGraphicsApi&());
    MOCK_METHOD1(AddTexture, Texture*(std::unique_ptr<Texture>));
};
}  // namespace GameEngine
