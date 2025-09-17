#include "EngineContext.h"

#include <Input/InputManager.h>

#include <memory>

#include "GameEngine/Renderers/RendererFactory.h"
#include "GameEngine/Resources/ResourceManagerFactory.h"
#include "Logger/Log.h"

namespace GameEngine
{
EngineContext::EngineContext(std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi,
                             std::unique_ptr<Physics::IPhysicsApi> physicsApi, std::unique_ptr<ISceneFactory> sceneFactory,
                             std::unique_ptr<IResourceManagerFactory> resourceManagerFactory,
                             std::unique_ptr<IRendererFactory> rendererFactory)
    : graphicsApi_(std::move(graphicsApi))
    , gpuResourceLoader_()  // gpuObjects can use graphicsApi should be deleted before
    , measurmentHandler_()
    , threadSync_(measurmentHandler_)
    , displayManager_(*graphicsApi_, measurmentHandler_)
    , inputManager_(displayManager_.GetInputManager())
    , resourceManagerFactory{resourceManagerFactory ? std::move(resourceManagerFactory)
                                                    : std::make_unique<ResourceManagerFactory>(*graphicsApi_, gpuResourceLoader_)}
    , physicsApi_(std::move(physicsApi))
    , renderersManager_(*graphicsApi_, gpuResourceLoader_, measurmentHandler_, threadSync_, displayManager_.GetTime(),
                        rendererFactory ? std::move(rendererFactory) : std::make_unique<RendererFactory>(*graphicsApi_))
    , sceneManager_{std::make_unique<SceneManager>(*this, std::move(sceneFactory))}
{
}

EngineContext::~EngineContext()
{
    LOG_DEBUG << "destructor";
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
