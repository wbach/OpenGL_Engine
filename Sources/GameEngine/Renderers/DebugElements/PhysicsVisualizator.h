#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include "GameEngine/Shaders/ShaderProgram.h"
#include <atomic>

namespace Utils
{
namespace Thread
{
class Worker;
}
}  // namespace Utils

namespace GameEngine
{
class PhysicsVisualizator
{
public:
    PhysicsVisualizator(GraphicsApi::IGraphicsApi& graphicsApi);
    ~PhysicsVisualizator();
    void Init();
    void Render();
    void SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()>);
    void ReloadShader();
    void Enable();
    void Disable();

private:
    void UpdatePhycisLineMesh();
    void UpdatePhysicsByWorker();
    bool IsReady() const;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    ShaderProgram shader_;

    std::function<const GraphicsApi::LineMesh&()> physicsDebugDraw_;

    uint32 refreshRateStepDown_;
    GraphicsApi::ID lineMeshId_;
    bool physicsLineMeshReady_;
    uint32 updatePhycisLineMesh_;
    bool isActive_;

    uint32 frameRefreshNumber_;
    Utils::Thread::Worker* worker_;
    std::atomic_bool isUpdated_;
    const GraphicsApi::LineMesh* lineMesh_;
    bool useWorkerToUpdate_;
};
}  // namespace GameEngine
