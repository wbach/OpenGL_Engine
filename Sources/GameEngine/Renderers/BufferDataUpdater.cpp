#include "BufferDataUpdater.h"

#include <Logger/Log.h>

#include <mutex>

#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "TransformDataEvent.h"

namespace GameEngine
{
void BufferDataUpdater::Subscribe(GameObject* gameObject)
{
    if (not gameObject)
    {
        LOG_ERROR << "gameObject not set!";
        return;
    }

    auto rendererComponents = gameObject->GetComponents<Components::RendererComponent>();
    if (rendererComponents.empty())
        return;

    std::lock_guard<std::mutex> lk(subsribtionMutex_);

    auto iter = subscribers_.find(gameObject->GetId());
    if (iter != subscribers_.end())
    {
        return;
    }

    AddEvent(gameObject->GetId(), TransformDataEvent{*gameObject});

    LOG_DEBUG << "SubscribeOnWorldTransfomChange " << *gameObject;
    auto subId = gameObject->SubscribeOnWorldTransfomChange(
        [gameObject, this](const auto&)
        {
            auto id = gameObject->GetId();
            AddEvent(id, TransformDataEvent{*gameObject});
        });

    subscribers_.insert(
        {gameObject->GetId(), BufferDataUpdaterSubscriber{.transformSubscribtionId = subId, .gameObject = gameObject}});
}
void BufferDataUpdater::UnSubscribe(GameObject* gameObject)
{
    LOG_DEBUG << "UnSubscribe " << gameObject->GetName();
    std::scoped_lock lk(eventMutex_, subsribtionMutex_);

    auto iter = subscribers_.find(gameObject->GetId());
    if (iter != subscribers_.end())
    {
        const auto& sub = iter->second;
        iter->second.gameObject->UnsubscribeOnWorldTransfromChange(sub.transformSubscribtionId);
        subscribers_.erase(iter);
    }

    events_.erase(gameObject->GetId());
    LOG_DEBUG << " BufferDataUpdater end" << gameObject->GetName();
}
void BufferDataUpdater::Update()
{
    std::lock_guard<std::mutex> lk(subsribtionMutex_);
    ProcessEvents();
}
void BufferDataUpdater::UnSubscribeAll()
{
    std::scoped_lock lk(eventMutex_, subsribtionMutex_);
    for (auto& [id, sub] : subscribers_)
    {
        sub.gameObject->UnsubscribeOnLocalTransfromChange(sub.transformSubscribtionId);
    }

    subscribers_.clear();
    events_.clear();
}
void BufferDataUpdater::ProcessEvents()
{
    BufferDataUpdaterEvents tmpEvents;
    {
        std::lock_guard<std::mutex> lk(eventMutex_);
        tmpEvents = std::move(events_);

        for (auto& [_, event] : tmpEvents)
        {
            event.Execute();
        }
    }

    for (auto& event : cameraUpdateEvents_)
    {
        event.Execute();
    }
    cameraUpdateEvents_.clear();
}
void BufferDataUpdater::AddEvent(uint32 gameobjectId, TransformDataEvent&& event)
{
    std::lock_guard<std::mutex> lk(eventMutex_);
    events_.insert_or_assign(gameobjectId, std::move(event));
}
void BufferDataUpdater::AddEvent(CameraBufferUpdateEvent&& event)
{
    cameraUpdateEvents_.push_back(std::move(event));
}
void BufferDataUpdater::UnSubscribe(const Components::IComponent& component)
{
    std::scoped_lock lk(eventMutex_, subsribtionMutex_);
    if (component.getParentGameObject().GetComponents<Components::RendererComponent>().size() > 1)
    {
        return;
    }

    auto ownerId = component.getParentGameObject().GetId();
    subscribers_.erase(ownerId);
    events_.erase(ownerId);
}
}  // namespace GameEngine
