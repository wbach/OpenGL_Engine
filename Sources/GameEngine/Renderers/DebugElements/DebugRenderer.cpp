#include "DebugRenderer.h"
#include <Logger/Log.h>
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
DebugRenderer::DebugRenderer(const RendererContext& context)
    : context_(context)
    , resourceManager_(context.graphicsApi_)
    , debugObjectShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::DebugObject)
    , gridShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Grid)
{
}

DebugRenderer::~DebugRenderer()
{
    DEBUG_LOG("");
}

void DebugRenderer::Init()
{
    debugObjectShader_.Init();
    gridShader_.Init();

    gridPerObjectUpdateBufferId_ =
        context_.graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

    if (gridPerObjectUpdateBufferId_)
    {
        PerObjectUpdate gridPerObjectUpdate;
        gridPerObjectUpdate.TransformationMatrix =
            Utils::CreateTransformationMatrix(vec3(0), DegreesVec3(-90, 0, 0), vec3(100));
        context_.graphicsApi_.UpdateShaderBuffer(*gridPerObjectUpdateBufferId_,
                                                 &gridPerObjectUpdate.TransformationMatrix);
    }
    else
    {
        ERROR_LOG("gridPerObjectUpdateBufferId_ error!");
    }

//    resourceManager_.LoadModel(EngineConf_GetFullDataPathAddToRequierd("Meshes/Indicator/Arrows.obj"));
}

void DebugRenderer::ReloadShaders()
{
    debugObjectShader_.Reload();
    gridShader_.Reload();
}

void DebugRenderer::Render(const Scene& scene, const Time&)
{
    context_.graphicsApi_.EnableBlend();
    context_.graphicsApi_.DisableCulling();

    DrawGrid();
    DrawDebugObjects();

    if (physicsDebugDraw_)
    {
        physicsDebugDraw_(scene.GetCamera().GetViewMatrix(), context_.projection_.GetProjectionMatrix());
    }

    context_.graphicsApi_.EnableCulling();
    context_.graphicsApi_.DisableBlend();
}

void GameEngine::DebugRenderer::SetPhysicsDebugDraw(std::function<void(const mat4&, const mat4&)> func)
{
    physicsDebugDraw_ = func;
}

void DebugRenderer::DrawGrid()
{
    if (not gridShader_.IsReady() and not gridPerObjectUpdateBufferId_)
        return;

    gridShader_.Start();
    context_.graphicsApi_.BindShaderBuffer(*gridPerObjectUpdateBufferId_);
    context_.graphicsApi_.RenderQuad();
    gridShader_.Stop();
}

void DebugRenderer::DrawDebugObjects()
{
    if (not debugObjectShader_.IsReady())
        return;

    debugObjectShader_.Start();

    for (const auto& debugObject : debugObjects_)
    {
        if (not debugObject.model)
            continue;

        if (debugObject.buffer.GetId())
        {
            context_.graphicsApi_.BindShaderBuffer(*debugObject.buffer.GetId());
        }

        RenderModel(*debugObject.model);
    }

    debugObjectShader_.Stop();
}

void DebugRenderer::RenderModel(const Model& model) const
{
    for (const auto& mesh : model.GetMeshes())
    {
        BindMeshBuffers(mesh);
        RenderMesh(mesh);
    }
}

void DebugRenderer::BindMeshBuffers(const Mesh& mesh) const
{
    const auto& perMeshObjectBuffer = mesh.GetBuffers().perMeshObjectBuffer_;
    if (perMeshObjectBuffer)
    {
        context_.graphicsApi_.BindShaderBuffer(*perMeshObjectBuffer);
    }
}

void DebugRenderer::RenderMesh(const Mesh& mesh) const
{
    context_.graphicsApi_.RenderMesh(mesh.GetGraphicsObjectId());
}
}  // namespace GameEngine
