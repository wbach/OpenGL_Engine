#include "PhysicsVisualizator.h"

namespace GameEngine
{
PhysicsVisualizator::PhysicsVisualizator(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , shader_(graphicsApi_, GraphicsApi::ShaderProgramType::Line)
    , refreshRateStepDown_(1)
    , isActive_(true)
    , frameRefreshNumber_(0)
{
}

void PhysicsVisualizator::Init()
{
    shader_.Init();
    lineMeshId_ = graphicsApi_.CreateDynamicLineMesh();
}

void PhysicsVisualizator::Render()
{
    if (not isActive_ or not IsReady())
        return;

    UpdatePhycisLineMesh();

    if (physicsLineMeshReady_)
    {
        shader_.Start();
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
    if (frameRefreshNumber_ > refreshRateStepDown_)
    {
        const auto& lineMesh = physicsDebugDraw_();

        if (not lineMesh.positions_.empty() and not lineMesh.colors_.empty())
        {
            graphicsApi_.UpdateLineMesh(*lineMeshId_, lineMesh);
            physicsLineMeshReady_ = true;
        }

        frameRefreshNumber_ = 1;
    }
    else
    {
        ++frameRefreshNumber_;
    }
}

bool PhysicsVisualizator::IsReady() const
{
    return physicsDebugDraw_ and lineMeshId_;
}

}  // namespace GameEngine
