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
    MOCK_METHOD1(LoadModel, Model*(const File&));
    MOCK_METHOD1(AddModelImpl, void(Model*));
    MOCK_METHOD1(ReleaseModel, void(Model&));
    MOCK_METHOD0(GetGpuResourceLoader, IGpuResourceLoader&());
    MOCK_METHOD0(GetTextureLoader, ITextureLoader&());
    MOCK_METHOD0(GetGraphicsApi, GraphicsApi::IGraphicsApi&());

    void AddModel(std::unique_ptr<Model> model) override
    {
        AddModelImpl(model.get());
        models_.push_back(std::move(model));
    }

private:
    std::vector<std::unique_ptr<Model>> models_;
};
}  // namespace GameEngine
