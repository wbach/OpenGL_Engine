#pragma once
#include <gmock/gmock.h>
#include "Types.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/TextureLoader.h"
#include "GameEngine/Resources/GpuResourceLoader.h"

namespace GameEngine
{
class ResourceManagerMock :public IResourceManager
{
public:
    MOCK_METHOD1(LoadModel, Model*(const std::string&));
    MOCK_METHOD1(AddModel, void(Model*));
    MOCK_METHOD1(GetModel, Model*(uint32));
    MOCK_METHOD0(GetGpuResourceLoader, IGpuResourceLoader&());
    MOCK_METHOD0(GetTextureLaoder, TextureLoader&());
    MOCK_METHOD0(GetGraphicsApi, IGraphicsApiPtr());
};
}  // GameEngine
