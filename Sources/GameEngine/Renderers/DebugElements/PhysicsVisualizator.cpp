#include "PhysicsVisualizator.h"

#include <Utils/ThreadSync.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
PhysicsVisualizator::PhysicsVisualizator(GraphicsApi::IGraphicsApi& graphicsApi, Utils::Thread::ThreadSync& threadSync)
    : graphicsApi_(graphicsApi)
    , threadSync_(threadSync)
    , shader_(graphicsApi_, GraphicsApi::ShaderProgramType::Line)
    , refreshRateStepDown_(1)
    , frameRefreshNumber_(0)
    , worker_(nullptr)
    , isUpdated_(true)
    , lineMesh_(nullptr)
{
}

PhysicsVisualizator::~PhysicsVisualizator()
{
    if (worker_)
    {
        threadSync_.RemoveWorker(*worker_);
        worker_ = nullptr;
    }
}

void PhysicsVisualizator::Init()
{
    useWorkerToUpdate_   = EngineConf.debugParams.physicsVisualizator.useWorkredToUpdatePhysicsVisualization_;
    refreshRateStepDown_ = EngineConf.debugParams.physicsVisualizator.refreshRateStepDown_;

    shader_.Init();
    lineMeshId_ = graphicsApi_.CreateDynamicLineMesh();

    if (useWorkerToUpdate_)
        worker_ = &threadSync_.AddWorker();

    PerObjectUpdate buffer;
    defaultPerObjectUpdateId_ =
        graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

    if (defaultPerObjectUpdateId_)
        graphicsApi_.UpdateShaderBuffer(*defaultPerObjectUpdateId_, &buffer);
}

void PhysicsVisualizator::Render()
{
    if (not IsReady())
        return;

    if (useWorkerToUpdate_)
    {
        UpdatePhysicsByWorker();
    }
    else
    {
        UpdatePhycisLineMesh();
    }

    if (physicsLineMeshReady_.load())
    {
        shader_.Start();
        graphicsApi_.BindShaderBuffer(*defaultPerObjectUpdateId_);
        graphicsApi_.RenderMesh(*lineMeshId_);
        shader_.Stop();
    }
}

void PhysicsVisualizator::SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()> func)
{
    physicsDebugDraw_ = func;
}

void PhysicsVisualizator::ReloadShader()
{
    shader_.Reload();
}

void PhysicsVisualizator::UpdatePhycisLineMesh()
{
    if (frameRefreshNumber_ >= refreshRateStepDown_)
    {
        lineMesh_ = &physicsDebugDraw_();
        if (not lineMesh_->positions_.empty() and not lineMesh_->colors_.empty())
        {
            graphicsApi_.UpdateLineMesh(*lineMeshId_, *lineMesh_);
            physicsLineMeshReady_.store(true);
        }
        else
        {
            physicsLineMeshReady_.store(false);
        }

        frameRefreshNumber_ = 0;
    }
    else
    {
        ++frameRefreshNumber_;
    }
}

void PhysicsVisualizator::UpdatePhysicsByWorker()
{
    if (not isUpdated_.load())
        return;

    auto task = [&]() {
        lineMesh_ = &physicsDebugDraw_();
        if (not lineMesh_->positions_.empty() and not lineMesh_->colors_.empty())
        {
            physicsLineMeshReady_.store(true);
        }
    };

    auto callback = [&]() { isUpdated_.store(true); };

    if (physicsLineMeshReady_)
    {
        if (not lineMesh_->positions_.empty() and not lineMesh_->colors_.empty())
            graphicsApi_.UpdateLineMesh(*lineMeshId_, *lineMesh_);
    }

    if (worker_)
    {
        isUpdated_.store(false);
        worker_->AddTask(task, callback);
    }
}

bool PhysicsVisualizator::IsReady() const
{
    return physicsDebugDraw_ and lineMeshId_ and defaultPerObjectUpdateId_;
}

}  // namespace GameEngine
