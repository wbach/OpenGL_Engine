#pragma once
#include <atomic>

#include "GameEngine/Shaders/ShaderProgram.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace Utils
{
namespace Thread
{
class Worker;
class ThreadSync;
}  // namespace Thread
}  // namespace Utils

namespace GameEngine
{
class PhysicsVisualizator
{
public:
    PhysicsVisualizator(GraphicsApi::IGraphicsApi&, Utils::Thread::ThreadSync&);
    ~PhysicsVisualizator();
    void Init();
    void Render();
    void SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()>);
    void ReloadShader();

private:
    void UpdatePhycisLineMesh();
    void UpdatePhysicsByWorker();
    bool IsReady() const;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    Utils::Thread::ThreadSync& threadSync_;
    ShaderProgram shader_;
    GraphicsApi::ID defaultPerObjectUpdateId_;

    std::function<const GraphicsApi::LineMesh&()> physicsDebugDraw_;

    uint32 refreshRateStepDown_;
    GraphicsApi::ID lineMeshId_;
    std::atomic_bool physicsLineMeshReady_;

    uint32 frameRefreshNumber_;
    Utils::Thread::Worker* worker_;
    std::atomic_bool isUpdated_;
    const GraphicsApi::LineMesh* lineMesh_;
    bool useWorkerToUpdate_;
};
}  // namespace GameEngine
