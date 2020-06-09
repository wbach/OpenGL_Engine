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

DebugRenderer::DebugRenderer(GraphicsApi::IGraphicsApi& graphicsApi, Utils::Thread::ThreadSync& threadSync)
    : graphicsApi_(graphicsApi)
    , physicsVisualizator_(graphicsApi, threadSync)
    , debugObjectShader_(graphicsApi_, GraphicsApi::ShaderProgramType::DebugObject)
    , gridShader_(graphicsApi_, GraphicsApi::ShaderProgramType::Grid)
    , lineShader_(graphicsApi_, GraphicsApi::ShaderProgramType::Line)
{
}

DebugRenderer::~DebugRenderer()
{
    DEBUG_LOG("");
}

void DebugRenderer::Init()
{
    physicsVisualizator_.Init();
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
}

void DebugRenderer::ReloadShaders()
{
    debugObjectShader_.Reload();
    gridShader_.Reload();
    physicsVisualizator_.ReloadShader();
}

void DebugRenderer::Render(const Scene&, const Time&)
{
    for (auto state : states_)
    {
        switch (state)
        {
            case RenderState::Physics:
                physicsVisualizator_.Render();
                break;
            case RenderState::Normals:
                DrawNormals();
                break;
            case RenderState::Grid:
                DrawGrid();
                break;
            case RenderState::Objects:
                DrawDebugObjects();
                break;
        }
    }
}

void DebugRenderer::SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()> func)
{
    physicsVisualizator_.SetPhysicsDebugDraw(func);
}

void DebugRenderer::AddDebugObject(Model& model, common::Transform& transform)
{
    std::lock_guard<std::mutex> lk(debugObjectsMutex_);
    debugObjects_.emplace_back(graphicsApi_, model, transform);
    toCreateDebugObjects_.push_back(&debugObjects_.back());
}

void DebugRenderer::Enable()
{
    if (stashedStates_.empty())
    {
        states_ = {RenderState::Objects/*, RenderState::Grid*/};
    }
    else
    {
        states_ = stashedStates_;
    }
}

void DebugRenderer::Disable()
{
    stashedStates_ = states_;
    states_.clear();
}

bool DebugRenderer::IsEnable() const
{
    return not states_.empty();
}

bool DebugRenderer::IsStateEnabled(DebugRenderer::RenderState state) const
{
    auto iter = std::find(states_.begin(), states_.end(), state);
    return iter != states_.end();
}

const std::vector<DebugRenderer::RenderState> &DebugRenderer::GetStates() const
{
    return states_;
}

void DebugRenderer::ClearDebugObjects()
{
    std::lock_guard<std::mutex> lk(debugObjectsMutex_);
    debugObjects_.clear();
}

void DebugRenderer::AddState(DebugRenderer::RenderState state)
{
    if (not IsStateEnabled(state))
    {
        states_.push_back(state);
    }
}

void DebugRenderer::RemoveState(DebugRenderer::RenderState state)
{
    auto iter = std::find(states_.begin(), states_.end(), state);

    if (iter != states_.end())
    {
        states_.erase(iter);
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
    std::lock_guard<std::mutex> lk(debugObjectsMutex_);
    for (auto& debugObject : debugObjects_)
    {
        if (debugObject.toUpdate_)
            debugObject.UpdateBuffer();
    }
}

void DebugRenderer::RenderDebugObjects()
{
    std::lock_guard<std::mutex> lk(debugObjectsMutex_);
    for (const auto& debugObject : debugObjects_)
    {
        if (not debugObject.perObjectBufferId)
            continue;

        debugObject.BindBuffer();
        RenderModel(debugObject.model_);
    }
}

void DebugRenderer::LoadDefaultPerObjectBuffer()
{
}

void DebugRenderer::DrawGrid()
{
    if (gridShader_.IsReady() and gridPerObjectUpdateBufferId_)
    {
        gridShader_.Start();
        graphicsApi_.BindShaderBuffer(*gridPerObjectUpdateBufferId_);
        graphicsApi_.RenderQuad();
        gridShader_.Stop();
    }
}

void DebugRenderer::DrawDebugObjects()
{
    graphicsApi_.EnableBlend();
    graphicsApi_.DisableCulling();

    if (debugObjectShader_.IsReady())
    {
        CreateDebugObjects();
        UpdateDebugObjectsIfNeeded();

        debugObjectShader_.Start();
        RenderDebugObjects();
        debugObjectShader_.Stop();
    }
    graphicsApi_.EnableCulling();
    graphicsApi_.DisableBlend();
}

void DebugRenderer::DrawNormals()
{
    if (lineShader_.IsReady())
    {
        lineShader_.Start();
        graphicsApi_.RenderDebugNormals();
        lineShader_.Stop();
    }
}

void DebugRenderer::RenderModel(const Model& model) const
{
    for (const auto& mesh : model.GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
        {
            BindMeshBuffers(mesh);
            graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
        }
    }
}

void DebugRenderer::BindMeshBuffers(const Mesh& mesh) const
{
    const auto& perMeshObjectBuffer = mesh.getShaderBufferId();
    if (perMeshObjectBuffer)
    {
        graphicsApi_.BindShaderBuffer(*perMeshObjectBuffer);
    }
}
}  // namespace GameEngine
