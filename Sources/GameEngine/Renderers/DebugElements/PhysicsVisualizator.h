#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class PhysicsVisualizator
{
public:
    PhysicsVisualizator(GraphicsApi::IGraphicsApi& graphicsApi);
    void Init();
    void Render();
    void SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()>);
    void ReloadShader();

private:
    void UpdatePhycisLineMesh();
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
};
}  // namespace GameEngine
