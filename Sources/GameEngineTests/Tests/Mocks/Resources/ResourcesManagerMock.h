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
    MOCK_METHOD1(LoadModel, Model*(const std::string&));
    MOCK_METHOD1(AddModel, void(Model*));
    MOCK_METHOD1(ReleaseModel, void(Model*));
    MOCK_METHOD1(GetModel, Model*(uint32));
    MOCK_METHOD0(GetGpuResourceLoader, IGpuResourceLoader&());
    MOCK_METHOD0(GetTextureLaoder, ITextureLoader&());
    MOCK_METHOD0(GetGraphicsApi, GraphicsApi::IGraphicsApi&());
    MOCK_METHOD1(AddTextureImpl, Texture*(Texture*));

    Texture* AddTexture(std::unique_ptr<Texture> t)
    {
        return AddTextureImpl(t.get());
    }
};
}  // namespace GameEngine
