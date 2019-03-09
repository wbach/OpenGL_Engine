#include "BufferDataUpdater.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"

namespace GameEngine
{
class TransformDataSubcriber : public IBufferDataUpdaterSubcriber
{
public:
    TransformDataSubcriber(uint32 goId, common::Transform& transform, Components::RendererComponent& renderComponent)
        : goId_(goId)
        , transform_(transform)
        , renderComponent_(renderComponent)
    {
    }

    virtual void Update() override
    {
        transform_.TakeSnapShoot();
        renderComponent_.UpdateBuffers();
    }

    virtual uint32 GetId() const override
    {
        return goId_;
    }

private:
    uint32 goId_;
    common::Transform& transform_;
    Components::RendererComponent& renderComponent_;
};

class BonesDataSubcriber : public IBufferDataUpdaterSubcriber
{
public:
    BonesDataSubcriber(GraphicsApi::IGraphicsApi& graphicsApi, uint32 goId, Components::RendererComponent& renderComponent)
        : goId_(goId)
        , graphicsApi_(graphicsApi)
    {
        model_ = renderComponent.GetModelWrapper().Get(LevelOfDetail::L1);
    }

    virtual void Update() override
    {
        int index = 0;

        PerPoseUpdate pose;
        for (auto& boneTransform : model_->GetBoneTransforms())
        {
            pose.bonesTransforms[index++] = graphicsApi_.PrepareMatrixToLoad(boneTransform);
        }

        for (auto& mesh : model_->GetMeshes())
        {
            mesh.UpdatePoseBuffer(&pose);
        }
    }

    virtual uint32 GetId() const override
    {
        return goId_;
    }

private:
    uint32 goId_;
    ModelRawPtr model_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
};

BufferDataUpdater::BufferDataUpdater(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
{
}
BufferDataUpdater::~BufferDataUpdater()
{
}
void BufferDataUpdater::Subscribe(GameObject* gameObject)
{
    if (gameObject->worldTransform.isDynamic_)
    {
        auto rendererComponent = gameObject->GetComponent<Components::RendererComponent>();
        if (rendererComponent)
        {
            subscribers_.emplace_back(
                new TransformDataSubcriber(gameObject->GetId(), gameObject->worldTransform, *rendererComponent));

            if (rendererComponent->GetModelWrapper().Get()->IsAnyMeshUseTransform())
            {
                subscribers_.emplace_back(new BonesDataSubcriber(graphicsApi_, gameObject->GetId(), *rendererComponent));
            }
        }
    }
}
void BufferDataUpdater::UnSubscribe(GameObject* gameObject)
{
    for (auto iter = subscribers_.begin(); iter != subscribers_.end();)
    {
        if ((**iter).GetId() == gameObject->GetId())
        {
            iter = subscribers_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}
void BufferDataUpdater::Update()
{
    for (auto& sub : subscribers_)
    {
        sub->Update();
    }
}
void BufferDataUpdater::UnSubscribeAll()
{
    subscribers_.clear();
}
}  // namespace GameEngine
