#include "LineMeshVisualizator.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Utils/IThreadSync.h>
#include <Utils/Worker.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
LineMeshVisualizator::LineMeshVisualizator(GraphicsApi::IGraphicsApi& graphicsApi, Utils::Thread::IThreadSync& threadSync)
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

LineMeshVisualizator::~LineMeshVisualizator()
{
    if (worker_)
    {
        threadSync_.RemoveWorker(*worker_);
        worker_ = nullptr;
    }
}

void LineMeshVisualizator::Init()
{
    useWorkerToUpdate_   = EngineConf.debugParams.linemeshVisualizator.useWorkredToUpdateVisualization_;
    refreshRateStepDown_ = EngineConf.debugParams.linemeshVisualizator.refreshRateStepDown_;

    shader_.Init();
    lineMeshId_ = graphicsApi_.CreateDynamicLineMesh();

    if (useWorkerToUpdate_)
        worker_ = &threadSync_.AddWorker();

    PerObjectUpdate buffer;
    defaultPerObjectUpdateId_ =
        graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate), GraphicsApi::DrawFlag::Dynamic);

    if (defaultPerObjectUpdateId_)
        graphicsApi_.UpdateShaderBuffer(*defaultPerObjectUpdateId_, &buffer);
}

void LineMeshVisualizator::Render()
{
    if (not IsReady())
        return;

    if (useWorkerToUpdate_)
    {
        UpdateByWorker();
    }
    else
    {
        UpdateLineMesh();
    }

    if (lineMeshReady_.load())
    {
        shader_.Start();
        graphicsApi_.BindShaderBuffer(*defaultPerObjectUpdateId_);
        graphicsApi_.RenderMesh(*lineMeshId_);
        shader_.Stop();
    }
}

void LineMeshVisualizator::SetMeshCreationFunction(std::function<const GraphicsApi::LineMesh&()> func)
{
    createLineMesh = func;
}

void LineMeshVisualizator::ReloadShader()
{
    shader_.Reload();
}

void LineMeshVisualizator::UpdateLineMesh()
{
    if (frameRefreshNumber_ >= refreshRateStepDown_)
    {
        lineMesh_ = &createLineMesh();
        if (not lineMesh_->positions_.empty() and not lineMesh_->colors_.empty())
        {
            graphicsApi_.UpdateLineMesh(*lineMeshId_, *lineMesh_);
            lineMeshReady_.store(true);
        }
        else
        {
            lineMeshReady_.store(false);
        }

        frameRefreshNumber_ = 0;
    }
    else
    {
        ++frameRefreshNumber_;
    }
}

void LineMeshVisualizator::UpdateByWorker()
{
    if (not isUpdated_.load())
        return;

    auto task = [&]()
    {
        lineMesh_ = &createLineMesh();
        if (not lineMesh_->positions_.empty() and not lineMesh_->colors_.empty())
        {
            lineMeshReady_.store(true);
        }
    };

    auto callback = [&]() { isUpdated_.store(true); };

    if (lineMeshReady_)
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

bool LineMeshVisualizator::IsReady() const
{
    return createLineMesh and lineMeshId_ and defaultPerObjectUpdateId_;
}

}  // namespace GameEngine
