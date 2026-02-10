#include "BufferDataUpdater.h"

#include <Logger/Log.h>

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

    auto rendererComponent = gameObject->GetComponent<Components::RendererComponent>();

    if (rendererComponent)
    {
        AddEvent(gameObject->GetId(), TransformDataEvent{*rendererComponent});

        LOG_DEBUG << "SubscribeOnWorldTransfomChange " << *gameObject;
        auto subId =
            gameObject->SubscribeOnWorldTransfomChange([id = gameObject->GetId(), this, rendererComponent](const auto&) mutable
                                                       { AddEvent(id, TransformDataEvent{*rendererComponent}); });

        std::lock_guard<std::mutex> lk(subsribtionMutex_);
        subscribers_.push_back(BufferDataUpdaterSubscriber{.transformSubscribtionId = subId, .gameObject = gameObject});
    }
}
void BufferDataUpdater::UnSubscribe(GameObject* gameObject)
{
    LOG_DEBUG << "UnSubscribe " << gameObject->GetName();
    bool result{false};

    std::lock_guard<std::mutex> lk(subsribtionMutex_);
    for (auto iter = subscribers_.begin(); iter != subscribers_.end();)
    {
        if (iter->gameObject and iter->gameObject->GetId() == gameObject->GetId())
        {
            gameObject->UnsubscribeOnWorldTransfromChange(iter->transformSubscribtionId);
            iter = subscribers_.erase(iter);
            LOG_DEBUG << "erase " << gameObject->GetName();
            result = true;
        }
        else
        {
            ++iter;
        }
    }

    if (not result)
    {
        LOG_DEBUG << "not erase " << gameObject->GetName() << " size = " << subscribers_.size();
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
    {
        std::lock_guard<std::mutex> lk(subsribtionMutex_);
        for (auto iter = subscribers_.begin(); iter != subscribers_.end();)
        {
            if (iter->gameObject)
            {
                iter->gameObject->UnsubscribeOnWorldTransfromChange(iter->transformSubscribtionId);
            }

            iter = subscribers_.erase(iter);
        }
    }

    std::lock_guard<std::mutex> eventLk(eventMutex_);
    events_.clear();
}
void BufferDataUpdater::ProcessEvents()
{
    BufferDataUpdaterEvents tmpEvents;
    {
        std::lock_guard<std::mutex> lk(eventMutex_);
        tmpEvents = std::move(events_);
    }

    for (auto& [_, event] : tmpEvents)
    {
        event.Execute();
    }

    for(auto& event : cameraUpdateEvents_)
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
}  // namespace GameEngine
