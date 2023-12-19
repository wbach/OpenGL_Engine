#include "BufferDataUpdater.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "TransformDataEvent.h"

namespace GameEngine
{
BufferDataUpdater::BufferDataUpdater(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
{
}
BufferDataUpdater::~BufferDataUpdater()
{
}
void BufferDataUpdater::Subscribe(GameObject* gameObject)
{
    if (not gameObject)
    {
        WARNING_LOG("gameObject not set!");
        return;
    }

    auto rendererComponent = gameObject->GetComponent<Components::RendererComponent>();
    auto jointPoseUpdaterPtr = gameObject->GetComponent<Components::JointPoseUpdater>();

    if (rendererComponent)
    {
        AddEvent(gameObject->GetId(), std::make_unique<TransformDataEvent>(*rendererComponent, jointPoseUpdaterPtr));

        gameObject->SubscribeOnWorldTransfomChange(
            [id = gameObject->GetId(), this, rendererComponent, jointPoseUpdaterPtr](const auto&) mutable {
                AddEvent(id, std::make_unique<TransformDataEvent>(*rendererComponent, jointPoseUpdaterPtr));
            });
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
    events_.push_back({gameobjectId, std::move(event)});
}
}  // namespace GameEngine
