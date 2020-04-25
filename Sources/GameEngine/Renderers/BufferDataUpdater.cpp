#include "BufferDataUpdater.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "TransformDataEvent.h"

namespace GameEngine
{
class BonesDataSubcriber : public IBufferDataUpdaterSubcriber
{
public:
    BonesDataSubcriber(GraphicsApi::IGraphicsApi& graphicsApi, uint32 goId,
                       Components::RendererComponent& renderComponent)
        : goId_(goId)
        , graphicsApi_(graphicsApi)
    {
        model_ = renderComponent.GetModelWrapper().Get(LevelOfDetail::L1);
    }
    ~BonesDataSubcriber() override = default;

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
    Model* model_;
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
    auto rendererComponent = gameObject->GetComponent<Components::RendererComponent>();

    if (rendererComponent)
    {
        rendererComponent->UpdateBuffers();

        //auto subscribtionId =
            gameObject->SubscribeOnWorldTransfomChange([this, rendererComponent](const auto&) mutable {
                AddEvent(std::make_unique<TransformDataEvent>(*rendererComponent));
            });

        if (rendererComponent->GetModelWrapper().Get()->IsAnyMeshUseTransform())
        {
            subscribers_.emplace_back(new BonesDataSubcriber(graphicsApi_, gameObject->GetId(), *rendererComponent));
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
    ProcessEvents();

    for (auto& sub : subscribers_)
    {
        sub->Update();
    }
}
void BufferDataUpdater::UnSubscribeAll()
{
    subscribers_.clear();
}
void BufferDataUpdater::ProcessEvents()
{
    std::lock_guard<std::mutex> lk(eventMutex_);
    for (auto& e : events_)
        e->Execute();
    events_.clear();
}
void BufferDataUpdater::AddEvent(std::unique_ptr<IBufferDataUpdaterEvent> event)
{
    std::lock_guard<std::mutex> lk(eventMutex_);
    events_.push_back(std::move(event));
}
}  // namespace GameEngine
