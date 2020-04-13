#include "DebugRenderer.h"

#include <Common/Transform.h>
#include <Logger/Log.h>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
DebugObject::DebugObject(GraphicsApi::IGraphicsApi& graphicsApi, Model& model, common::Transform& transform)
    : graphicsApi_(graphicsApi)
    , model_(model)
    , transform_(transform)
    , toUpdate_(true)
{
}
DebugObject::~DebugObject()
{
    if (perObjectBufferId)
        graphicsApi_.DeleteShaderBuffer(*perObjectBufferId);
}
void DebugObject::CreateBuffer()
{
    DEBUG_LOG("perObjectBufferId");
    perObjectBufferId = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

    DEBUG_LOG("perObjectBufferId");
    transform_.TakeSnapShoot();
    buffer.TransformationMatrix = transform_.GetMatrix();
    UpdateBuffer();

    toUpdate_ = false;

    transform_.SubscribeOnChange([&](const common::Transform&) {
        transform_.TakeSnapShoot();
        buffer.TransformationMatrix = transform_.GetMatrix();
        toUpdate_                   = true;
    });
}
void DebugObject::UpdateBuffer()
{
    if (perObjectBufferId)
    {
        graphicsApi_.UpdateShaderBuffer(*perObjectBufferId, &buffer);
        toUpdate_ = false;
    }
}
void DebugObject::BindBuffer() const
{
    if (perObjectBufferId)
    {
        graphicsApi_.BindShaderBuffer(*perObjectBufferId);
    }
}

DebugRenderer::DebugRenderer(GraphicsApi::IGraphicsApi& graphicsApi, Projection& projection)
    : graphicsApi_(graphicsApi)
    , projection_(projection)
    , debugObjectShader_(graphicsApi_, GraphicsApi::ShaderProgramType::DebugObject)
    , gridShader_(graphicsApi_, GraphicsApi::ShaderProgramType::Grid)
    , lineShader_(graphicsApi_, GraphicsApi::ShaderProgramType::Line)
    , isActive_(false)
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
    lineShader_.Init();

    gridPerObjectUpdateBufferId_ =
        graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

    if (gridPerObjectUpdateBufferId_)
    {
        PerObjectUpdate gridPerObjectUpdate;
        gridPerObjectUpdate.TransformationMatrix =
            Utils::CreateTransformationMatrix(vec3(0), DegreesVec3(-90, 0, 0), vec3(100));
        graphicsApi_.UpdateShaderBuffer(*gridPerObjectUpdateBufferId_, &gridPerObjectUpdate);
    }
    else
    {
        ERROR_LOG("gridPerObjectUpdateBufferId_ error!");
    }

    lineMeshId_ = graphicsApi_.CreateDynamicLineMesh();
}

void DebugRenderer::ReloadShaders()
{
    debugObjectShader_.Reload();
    gridShader_.Reload();
}

void DebugRenderer::Render(const Scene& scene, const Time&)
{
    if (not isActive_)
        return;

    graphicsApi_.EnableBlend();
    graphicsApi_.DisableCulling();

    DrawGrid();
    DrawDebugObjects();
    DrawPhysics(scene.GetCamera().GetViewMatrix());

    graphicsApi_.EnableCulling();
    graphicsApi_.DisableBlend();
}

void GameEngine::DebugRenderer::SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()> func)
{
    physicsDebugDraw_ = func;
}

void DebugRenderer::AddDebugObject(Model& model, common::Transform& transform)
{
    debugObjects_.emplace_back(graphicsApi_, model, transform);
    toCreateDebugObjects_.push_back(&debugObjects_.back());
}

void DebugRenderer::Enable()
{
    isActive_ = true;
}

void DebugRenderer::Disable()
{
    isActive_ = false;
}

void DebugRenderer::DrawPhysics(const mat4& viewMatrix)
{
    if (physicsDebugDraw_ and lineMeshId_)
    {
        const auto& lineMesh = physicsDebugDraw_();

        if (not lineMesh.positions_.empty() and not lineMesh.colors_.empty())
        {
            if (lineMesh.updateBufferNeeded)
            {
                graphicsApi_.UpdateLineMesh(*lineMeshId_, lineMesh);
            }

            lineShader_.Start();
            graphicsApi_.RenderMesh(*lineMeshId_);
            lineShader_.Stop();
        }
    }
}

void DebugRenderer::CreateDebugObjects()
{
    for (auto iter = toCreateDebugObjects_.begin(); iter != toCreateDebugObjects_.end();)
    {
        (*iter)->CreateBuffer();
        iter = toCreateDebugObjects_.erase(iter);
    }
}

void DebugRenderer::UpdateDebugObjectsIfNeeded()
{
    for (auto& debugObject : debugObjects_)
    {
        if (debugObject.toUpdate_)
            debugObject.UpdateBuffer();
    }
}

void DebugRenderer::DrawGrid()
{
    if (not gridShader_.IsReady() and not gridPerObjectUpdateBufferId_)
        return;

    gridShader_.Start();
    graphicsApi_.BindShaderBuffer(*gridPerObjectUpdateBufferId_);
    graphicsApi_.RenderQuad();
    gridShader_.Stop();
}

void DebugRenderer::DrawDebugObjects()
{
    if (not debugObjectShader_.IsReady())
        return;

    CreateDebugObjects();
    UpdateDebugObjectsIfNeeded();

    debugObjectShader_.Start();

    for (const auto& debugObject : debugObjects_)
    {
        if (not debugObject.perObjectBufferId)
            continue;

        debugObject.BindBuffer();
        RenderModel(debugObject.model_);
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
        graphicsApi_.BindShaderBuffer(*perMeshObjectBuffer);
    }
}

void DebugRenderer::RenderMesh(const Mesh& mesh) const
{
    graphicsApi_.RenderMesh(mesh.GetGraphicsObjectId());
}
}  // namespace GameEngine
