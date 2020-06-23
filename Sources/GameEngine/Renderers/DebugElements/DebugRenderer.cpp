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
namespace
{
struct ColorBuffer
{
    AlignWrapper<vec4> color;
};
}  // namespace
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

DebugRenderer::DebugRenderer(RendererContext& rendererContext, Utils::Thread::ThreadSync& threadSync)
    : rendererContext_(rendererContext)
    , physicsVisualizator_(rendererContext.graphicsApi_, threadSync)
    , debugObjectShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::DebugObject)
    , gridShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::Grid)
    , lineShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::Line)
    , textureShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::Texture)
{
}

DebugRenderer::~DebugRenderer()
{
    DEBUG_LOG("");
}

void DebugRenderer::init()
{
    physicsVisualizator_.Init();
    debugObjectShader_.Init();
    gridShader_.Init();
    lineShader_.Init();
    textureShader_.Init();

    gridPerObjectUpdateBufferId_ =
        rendererContext_.graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

    if (gridPerObjectUpdateBufferId_)
    {
        PerObjectUpdate gridPerObjectUpdate;
        gridPerObjectUpdate.TransformationMatrix =
            Utils::CreateTransformationMatrix(vec3(0), DegreesVec3(-90, 0, 0), vec3(100));
        rendererContext_.graphicsApi_.UpdateShaderBuffer(*gridPerObjectUpdateBufferId_, &gridPerObjectUpdate);
    }
    else
    {
        ERROR_LOG("gridPerObjectUpdateBufferId_ error!");
    }

    texturePerObjectUpdateBufferId_ =
        rendererContext_.graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

    if (texturePerObjectUpdateBufferId_)
    {
        PerObjectUpdate gridPerObjectUpdate;
        gridPerObjectUpdate.TransformationMatrix = Utils::CreateTransformationMatrix(vec3(0), DegreesVec3(0), vec3(1));
        rendererContext_.graphicsApi_.UpdateShaderBuffer(*texturePerObjectUpdateBufferId_, &gridPerObjectUpdate);
    }
    else
    {
        ERROR_LOG("texturePerObjectUpdateBufferId_ error!");
    }

    textureColorBufferId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(ColorBuffer));
    if (textureColorBufferId_)
    {
        ColorBuffer b;
        b.color = vec4(1.f);
        rendererContext_.graphicsApi_.UpdateShaderBuffer(*textureColorBufferId_, &b);
    }
}

void DebugRenderer::reloadShaders()
{
    debugObjectShader_.Reload();
    gridShader_.Reload();
    physicsVisualizator_.ReloadShader();
}

void DebugRenderer::render()
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

void DebugRenderer::renderTextures(const std::vector<GraphicsApi::ID>& textures)
{
    if (texturePerObjectUpdateBufferId_)
    {
        rendererContext_.graphicsApi_.DisableDepthTest();
        rendererContext_.graphicsApi_.DisableDepthMask();

        textureShader_.Start();
        rendererContext_.graphicsApi_.BindShaderBuffer(*texturePerObjectUpdateBufferId_);
        rendererContext_.graphicsApi_.BindShaderBuffer(*textureColorBufferId_);
        float i = 0;
        const vec2 size(0.25f);
        rendererContext_.graphicsApi_.ActiveTexture(0);
        for (auto& textureId : textures)
        {
            if (textureId)
            {
                auto pos = vec3(-.99f + (size.x), .99f - size.y - (2.f * size.y * i), 0);
                PerObjectUpdate gridPerObjectUpdate;
                gridPerObjectUpdate.TransformationMatrix =
                    Utils::CreateTransformationMatrix(pos, size, DegreesFloat(0.f));
                rendererContext_.graphicsApi_.UpdateShaderBuffer(*texturePerObjectUpdateBufferId_, &gridPerObjectUpdate);

                rendererContext_.graphicsApi_.BindTexture(*textureId);
                rendererContext_.graphicsApi_.RenderQuad();
                ++i;
            }
        }
        textureShader_.Stop();

        rendererContext_.graphicsApi_.EnableDepthMask();
        rendererContext_.graphicsApi_.EnableDepthTest();
    }
}

void DebugRenderer::SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()> func)
{
    physicsVisualizator_.SetPhysicsDebugDraw(func);
}

void DebugRenderer::AddDebugObject(Model& model, common::Transform& transform)
{
    std::lock_guard<std::mutex> lk(debugObjectsMutex_);
    debugObjects_.emplace_back(rendererContext_.graphicsApi_, model, transform);
    toCreateDebugObjects_.push_back(&debugObjects_.back());
}

void DebugRenderer::Enable()
{
    if (stashedStates_.empty())
    {
        states_ = {RenderState::Objects /*, RenderState::Grid*/};
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

const std::vector<DebugRenderer::RenderState>& DebugRenderer::GetStates() const
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
        rendererContext_.graphicsApi_.BindShaderBuffer(*gridPerObjectUpdateBufferId_);
        rendererContext_.graphicsApi_.RenderQuad();
        gridShader_.Stop();
    }
}

void DebugRenderer::DrawDebugObjects()
{
    rendererContext_.graphicsApi_.EnableBlend();
    rendererContext_.graphicsApi_.DisableCulling();

    if (debugObjectShader_.IsReady())
    {
        CreateDebugObjects();
        UpdateDebugObjectsIfNeeded();

        debugObjectShader_.Start();
        RenderDebugObjects();
        debugObjectShader_.Stop();
    }
    rendererContext_.graphicsApi_.EnableCulling();
    rendererContext_.graphicsApi_.DisableBlend();
}

void DebugRenderer::DrawNormals()
{
    if (lineShader_.IsReady())
    {
        lineShader_.Start();
        rendererContext_.graphicsApi_.RenderDebugNormals();
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
            rendererContext_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
        }
    }
}

void DebugRenderer::BindMeshBuffers(const Mesh& mesh) const
{
    const auto& perMeshObjectBuffer = mesh.getShaderBufferId();
    if (perMeshObjectBuffer)
    {
        rendererContext_.graphicsApi_.BindShaderBuffer(*perMeshObjectBuffer);
    }
}
}  // namespace GameEngine
