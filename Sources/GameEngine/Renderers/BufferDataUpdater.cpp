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
        AddEvent(gameObject->GetId(), std::make_unique<TransformDataEvent>(*rendererComponent));

        LOG_DEBUG << "SubscribeOnWorldTransfomChange " << *gameObject;
        auto subId = gameObject->SubscribeOnWorldTransfomChange(
            [id = gameObject->GetId(), this, rendererComponent](const auto&) mutable
            { AddEvent(id, std::make_unique<TransformDataEvent>(*rendererComponent)); });

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
    LOG_DEBUG << " BufferDataUpdater end" << gameObject->GetName();
}
void BufferDataUpdater::Update()
{
    std::lock_guard<std::mutex> lk(subsribtionMutex_);
    ProcessEvents();
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
