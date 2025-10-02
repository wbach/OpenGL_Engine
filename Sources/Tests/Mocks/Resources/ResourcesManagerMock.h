#pragma once
#include <gmock/gmock.h>

#include <memory>
#include <vector>

#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/Models/Model.h"

namespace GameEngine
{
class ResourceManagerMock : public IResourceManager
{
public:
    MOCK_METHOD2(LoadModel, Model*(const File&, const LoadingParameters&));
    MOCK_METHOD(Model*, GetPrimitives, (PrimitiveType), (override));
    MOCK_METHOD1(AddModelImpl, Model*(Model*));
    MOCK_METHOD1(ReleaseModel, void(Model&));
    MOCK_METHOD0(GetGpuResourceLoader, IGpuResourceLoader&());
    MOCK_METHOD0(GetTextureLoader, ITextureLoader&());
    MOCK_METHOD0(GetGraphicsApi, GraphicsApi::IGraphicsApi&());
    MOCK_METHOD0(LockReleaseResources, void());
    MOCK_METHOD0(UnlockReleaseResources, void());

    Model* AddModel(std::unique_ptr<Model> model) override
    {
        auto ptr = model.get();
        AddModelImpl(ptr);
        models_.push_back(std::move(model));
        return ptr;
    }

private:
    std::vector<std::unique_ptr<Model>> models_;
};
}  // namespace GameEngine
