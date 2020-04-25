#include "EngineContext.h"
#include <Input/InputManager.h>
#include "Configuration.h"

namespace GameEngine
{
EngineContext::EngineContext(std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi,
                             std::unique_ptr<Physics::IPhysicsApi> physicsApi)
    : measurmentHandler_()
    , graphicsApi_(std::move(graphicsApi))
    , physicsApi_(std::move(physicsApi))
    , displayManager_(*graphicsApi_, measurmentHandler_)
    , inputManager_(displayManager_.CreateInput())
    , threadSync_(measurmentHandler_)
    , renderersManager_(*graphicsApi_, measurmentHandler_, threadSync_)
{
}

EngineContext::~EngineContext()
{
    DEBUG_LOG("destructor");
    threadSync_.Stop();
}

void EngineContext::AddEngineEvent(EngineEvent event)
{
    std::lock_guard<std::mutex> lk(engineEventsMutex_);
    engineEvents_.push_back(event);
}

std::optional<EngineEvent> EngineContext::GetEngineEvent()
{
    if (engineEvents_.empty())
        return std::nullopt;

    std::lock_guard<std::mutex> lk(engineEventsMutex_);

    auto event = engineEvents_.front();
    engineEvents_.pop_front();

    return event;
}
}  // namespace GameEngine
