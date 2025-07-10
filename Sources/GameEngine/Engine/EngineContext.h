#pragma once
#include <Types.h>
#include <Utils/MeasurementHandler.h>
#include <Utils/ThreadSync.h>
#include <Utils/Time/TimerService.h>

#include <Mutex.hpp>
#include <list>
#include <memory>
#include <unordered_map>

#include "EngineEvent.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "MeasurementHandler.h"

namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi
namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class DisplayManager;
namespace Renderer
{
class RenderersManager;
}  // namespace Renderer

class EngineContext
{
public:
    EngineContext(std::unique_ptr<GraphicsApi::IGraphicsApi>, std::unique_ptr<Physics::IPhysicsApi>);
    ~EngineContext();

    void AddEngineEvent(EngineEvent);
    std::optional<EngineEvent> GetEngineEvent();

    inline Utils::MeasurementHandler& GetMeasurmentHandler();
    inline DisplayManager& GetDisplayManager();
    inline Physics::IPhysicsApi& GetPhysicsApi();
    inline Input::InputManager& GetInputManager();
    inline IGpuResourceLoader& GetGpuResourceLoader();
    inline Utils::Thread::ThreadSync& GetThreadSync();
    inline GraphicsApi::IGraphicsApi& GetGraphicsApi();
    inline Renderer::RenderersManager& GetRenderersManager();
    inline Utils::Time::TimerService& GetTimerService();

private:
    Utils::MeasurementHandler measurmentHandler_;
    std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi_;
    std::unique_ptr<Physics::IPhysicsApi> physicsApi_;
    DisplayManager displayManager_;
    Input::InputManager& inputManager_;
    Utils::Thread::ThreadSync threadSync_;
    Utils::Time::TimerService timerService_;

    GpuResourceLoader gpuResourceLoader_;
    Renderer::RenderersManager renderersManager_;

    std::mutex engineEventsMutex_;
    std::list<EngineEvent> engineEvents_;
};

Utils::MeasurementHandler& EngineContext::GetMeasurmentHandler()
{
    return measurmentHandler_;
}

DisplayManager& EngineContext::GetDisplayManager()
{
    return displayManager_;
}

Physics::IPhysicsApi& EngineContext::GetPhysicsApi()
{
    return *physicsApi_;
}

Input::InputManager& EngineContext::GetInputManager()
{
    return inputManager_;
}

IGpuResourceLoader& EngineContext::GetGpuResourceLoader()
{
    return gpuResourceLoader_;
}

Utils::Thread::ThreadSync& EngineContext::GetThreadSync()
{
    return threadSync_;
}

GraphicsApi::IGraphicsApi& EngineContext::GetGraphicsApi()
{
    return *graphicsApi_;
}

Renderer::RenderersManager& EngineContext::GetRenderersManager()
{
    return renderersManager_;
}
Utils::Time::TimerService& EngineContext::GetTimerService()
{
    return timerService_;
}
}  // namespace GameEngine
