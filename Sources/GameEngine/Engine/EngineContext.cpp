#include "EngineContext.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <memory>

#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Renderers/RendererFactory.h"
#include "GameEngine/Resources/ResourceManagerFactory.h"
#include "ThreadSync.h"

namespace GameEngine
{
EngineContext::EngineContext(std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi,
                             std::unique_ptr<Physics::IPhysicsApi> physicsApi, std::unique_ptr<ISceneFactory> sceneFactory)
    : graphicsApi_(std::move(graphicsApi))
    , gpuResourceLoader_()  // gpuObjects can use graphicsApi should be deleted before
    , measurmentHandler_()
    , threadSync_(std::make_unique<Utils::Thread::ThreadSync>(measurmentHandler_))
    , displayManager_(*graphicsApi_, measurmentHandler_)
    , inputManager_(displayManager_.GetInputManager())
    , resourceManagerFactory{std::make_unique<ResourceManagerFactory>(*graphicsApi_, gpuResourceLoader_)}
    , physicsApi_(std::move(physicsApi))
    , renderersManager_(*graphicsApi_, gpuResourceLoader_, measurmentHandler_, *threadSync_, displayManager_.GetTime(),
                        std::make_unique<RendererFactory>(*graphicsApi_))
    , sceneManager_{std::make_unique<SceneManager>(*this, std::move(sceneFactory))}
{
}

EngineContext::EngineContext(std::unique_ptr<Physics::IPhysicsApi> physicsApi, std::unique_ptr<ISceneFactory> sceneFactory,
                             std::unique_ptr<IResourceManagerFactory> resourceManagerFactory,
                             std::unique_ptr<IRendererFactory> rendererFactory,
                             std::unique_ptr<Utils::Thread::IThreadSync> threadSync,
                             std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi)
    : graphicsApi_(std::move(graphicsApi))
    , gpuResourceLoader_()  // gpuObjects can use graphicsApi should be deleted before
    , measurmentHandler_()
    , threadSync_(std::move(threadSync))
    , displayManager_(*graphicsApi_, measurmentHandler_)
    , inputManager_(displayManager_.GetInputManager())
    , resourceManagerFactory{std::move(resourceManagerFactory)}
    , physicsApi_(std::move(physicsApi))
    , renderersManager_(*graphicsApi_, gpuResourceLoader_, measurmentHandler_, *threadSync_, displayManager_.GetTime(),
                        std::move(rendererFactory))
    , sceneManager_{std::make_unique<SceneManager>(*this, std::move(sceneFactory))}
{
}

EngineContext::~EngineContext()
{
    LOG_DEBUG << "destructor";
    threadSync_->Stop();
}

void EngineContext::AddEngineEvent(EngineEvent event)
{
    std::lock_guard<std::mutex> lk(engineEventsMutex_);
    engineEvents_.push_back(event);
}

EngineContext::EngineEvents EngineContext::GetEngineEvents()
{
    std::lock_guard<std::mutex> lk(engineEventsMutex_);
    return std::move(engineEvents_);
}
}  // namespace GameEngine
