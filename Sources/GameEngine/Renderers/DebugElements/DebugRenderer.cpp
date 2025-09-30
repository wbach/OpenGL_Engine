#include "DebugRenderer.h"

#include <Common/Transform.h>
#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
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
    /* LOG TO FIX*/  LOG_ERROR << ("perObjectBufferId");
    perObjectBufferId = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

    /* LOG TO FIX*/  LOG_ERROR << ("perObjectBufferId");
    transform_.TakeSnapShoot();
    buffer.TransformationMatrix = transform_.GetMatrix();
    UpdateBuffer();
    toUpdate_ = false;

    transform_.SubscribeOnChange(
        [&](const common::Transform&)
        {
            buffer.TransformationMatrix = graphicsApi_.PrepareMatrixToLoad(transform_.CalculateCurrentMatrix());
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
    , debugNormalShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::DebugNormal)
    , textureShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::Texture)
{
}

DebugRenderer::~DebugRenderer()
{
    /* LOG TO FIX*/  LOG_ERROR << ("");
    if (showPhycicsVisualizationSubId)
        EngineConf.debugParams.showPhycicsVisualization.unsubscribe(*showPhycicsVisualizationSubId);
}

void DebugRenderer::init()
{
    physicsVisualizator_.Init();
    debugObjectShader_.Init();
    gridShader_.Init();
    debugNormalShader_.Init();
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
        /* LOG TO FIX*/  LOG_ERROR << ("gridPerObjectUpdateBufferId_ error!");
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
        /* LOG TO FIX*/  LOG_ERROR << ("texturePerObjectUpdateBufferId_ error!");
    }

    textureColorBufferId_ =
        rendererContext_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(ColorBuffer));
    if (textureColorBufferId_)
    {
        ColorBuffer b;
        b.color = vec4(1.f);
        rendererContext_.graphicsApi_.UpdateShaderBuffer(*textureColorBufferId_, &b);
    }

    meshDebugPerObjectBufferId_ =
        rendererContext_.graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

    showPhycicsVisualizationSubId = EngineConf.debugParams.showPhycicsVisualization.subscribeForChange(
        [this]()
        {
            if (EngineConf.debugParams.showPhycicsVisualization)
            {
                AddState(DebugRenderer::RenderState::Physics);
            }
            else
            {
                RemoveState(DebugRenderer::RenderState::Physics);
            }
        });
}

void DebugRenderer::reloadShaders()
{
    debugObjectShader_.Reload();
    gridShader_.Reload();
    physicsVisualizator_.ReloadShader();
    debugNormalShader_.Reload();
}

void DebugRenderer::render()
{
    rendererContext_.graphicsApi_.DisableDepthTest();
    rendererContext_.graphicsApi_.DisableDepthMask();
    rendererContext_.graphicsApi_.DisableCulling();

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

void DebugRenderer::clear()
{
    states_.clear();
    clearDebugObjects();
}

void DebugRenderer::subscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(meshInfoDebugObjectsMutex_);
    auto terrain = gameObject.GetComponent<Components::TerrainRendererComponent>();

    if (terrain and terrain->GetRendererType() == Components::TerrainRendererComponent::RendererType::Mesh)
    {
        auto terrainMeshComponent = terrain->GetMeshTerrain();
        if (terrainMeshComponent)
        {
            meshDebugInfoSubscribers_.insert({gameObject.GetId(), {gameObject, terrainMeshComponent->GetModel()}});
        }
    }
    auto rc = gameObject.GetComponent<Components::RendererComponent>();

    if (rc)
    {
        meshDebugInfoSubscribers_.insert({gameObject.GetId(), {gameObject, rc->GetModelWrapper()}});
    }
}

void DebugRenderer::unSubscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(meshInfoDebugObjectsMutex_);

    auto iter = meshDebugInfoSubscribers_.find(gameObject.GetId());

    if (iter != meshDebugInfoSubscribers_.end())
    {
        meshDebugInfoSubscribers_.erase(iter);
    }
}

void DebugRenderer::unSubscribeAll()
{
    std::lock_guard<std::mutex> lk(meshInfoDebugObjectsMutex_);
    meshDebugInfoSubscribers_.clear();
}

void DebugRenderer::renderTextures(const std::vector<GraphicsApi::ID>& textures)
{
    if (texturePerObjectUpdateBufferId_)
    {
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
                gridPerObjectUpdate.TransformationMatrix = rendererContext_.graphicsApi_.PrepareMatrixToLoad(
                    Utils::CreateTransformationMatrix(pos, size, DegreesFloat(180.f)));
                rendererContext_.graphicsApi_.UpdateShaderBuffer(*texturePerObjectUpdateBufferId_,
                                                                 &gridPerObjectUpdate);

                rendererContext_.graphicsApi_.BindTexture(*textureId);
                rendererContext_.graphicsApi_.RenderQuad();
                ++i;
            }
        }
        textureShader_.Stop();
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

void DebugRenderer::clearDebugObjects()
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
    std::lock_guard<std::mutex> lk(debugObjectsMutex_);
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
    if (debugObjectShader_.IsReady())
    {
        CreateDebugObjects();
        UpdateDebugObjectsIfNeeded();

        debugObjectShader_.Start();
        RenderDebugObjects();
        debugObjectShader_.Stop();
    }
}

void DebugRenderer::DrawNormals()
{
    if (debugNormalShader_.IsReady() and meshDebugPerObjectBufferId_)
    {
        debugNormalShader_.Start();
        std::lock_guard<std::mutex> lk(meshInfoDebugObjectsMutex_);
        for (auto& [id, debugInfoMesh] : meshDebugInfoSubscribers_)
        {
            auto model = debugInfoMesh.modelWrapper.Get(LevelOfDetail::L1);
            if (model)
            {
                for (auto& mesh : model->GetMeshes())
                {
                    auto meshId = mesh.GetGraphicsObjectId();
                    if (meshId)
                    {
                        PerObjectUpdate update{rendererContext_.graphicsApi_.PrepareMatrixToLoad(
                            debugInfoMesh.gameObject.GetWorldTransform().CalculateCurrentMatrix() *
                            mesh.GetMeshTransform())};

                        rendererContext_.graphicsApi_.UpdateShaderBuffer(*meshDebugPerObjectBufferId_, &update);
                        rendererContext_.graphicsApi_.BindShaderBuffer(*meshDebugPerObjectBufferId_);
                        rendererContext_.graphicsApi_.RenderDebugNormals(*meshId);
                    }
                }
            }
        }
        debugNormalShader_.Stop();
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
    const auto& perMeshObjectBuffer = mesh.GetMaterialShaderBufferId();
    if (perMeshObjectBuffer)
    {
        rendererContext_.graphicsApi_.BindShaderBuffer(*perMeshObjectBuffer);
    }
}
}  // namespace GameEngine
