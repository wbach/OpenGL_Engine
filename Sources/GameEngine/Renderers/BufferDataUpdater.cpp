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

        auto subscribtionId =
            gameObject->SubscribeOnWorldTransfomChange([id = gameObject->GetId(), this, rendererComponent](const auto&) mutable {
                AddEvent(id, std::make_unique<TransformDataEvent>(*rendererComponent));
            });

        if (rendererComponent->GetModelWrapper().Get()->IsAnyMeshUseTransform())
        {
            std::lock_guard<std::mutex> lk(subsribtionMutex_);
            subscribers_.push_back({subscribtionId, std::make_unique<BonesDataSubcriber>(
                                                        graphicsApi_, gameObject->GetId(), *rendererComponent)});
        }
    }
}
void BufferDataUpdater::UnSubscribe(GameObject* gameObject)
{
    std::lock_guard<std::mutex> lk(subsribtionMutex_);
    for (auto iter = subscribers_.begin(); iter != subscribers_.end();)
    {
        if (iter->bufferDataUpdater_->GetId() == gameObject->GetId())
        {
            gameObject->UnsubscribeOnWorldTransfromChange(iter->transformSubscribtionId);
            iter = subscribers_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    for (auto iter = events_.begin(); iter != events_.end();)
    {
        if (iter->first == gameObject->GetId())
        {
            iter = events_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}
void BufferDataUpdater::Update()
{
    std::lock_guard<std::mutex> lk(subsribtionMutex_);
    ProcessEvents();
    for (auto& sub : subscribers_)
    {
        sub.bufferDataUpdater_->Update();
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
        e.second->Execute();
    events_.clear();
}
void BufferDataUpdater::AddEvent(uint32 gameobjectId, std::unique_ptr<IBufferDataUpdaterEvent> event)
{
    std::lock_guard<std::mutex> lk(eventMutex_);
    events_.push_back({ gameobjectId, std::move(event) });
}
}  // namespace GameEngine
