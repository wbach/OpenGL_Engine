#include "DebugRenderer.h"

#include <Common/Transform.h>
#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>
#include <Utils/Variant.h>

#include <algorithm>
#include <variant>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/DebugTools/Common/MouseUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
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
GraphicsApi::LineMesh CreateLineMeshFromBoundingBox(const GameEngine::BoundingBox& box, const glm::vec3& color = glm::vec3(1.0f))
{
    GraphicsApi::LineMesh mesh;

    // 8 wierzchołków AABB
    glm::vec3 min = box.min();
    glm::vec3 max = box.max();

    std::vector<glm::vec3> vertices = {
        {min.x, min.y, min.z},  // 0
        {max.x, min.y, min.z},  // 1
        {max.x, max.y, min.z},  // 2
        {min.x, max.y, min.z},  // 3
        {min.x, min.y, max.z},  // 4
        {max.x, min.y, max.z},  // 5
        {max.x, max.y, max.z},  // 6
        {min.x, max.y, max.z}   // 7
    };

    // 12 krawędzi AABB, każda jako dwa wierzchołki
    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},  // dolna płaszczyzna
        {4, 5}, {5, 6}, {6, 7}, {7, 4},  // górna płaszczyzna
        {0, 4}, {1, 5}, {2, 6}, {3, 7}   // pionowe krawędzie
    };

    // wypełniamy LineMesh
    for (auto& e : edges)
    {
        glm::vec3 v0 = vertices[e.first];
        glm::vec3 v1 = vertices[e.second];

        // pozycje
        mesh.positions_.push_back(v0.x);
        mesh.positions_.push_back(v0.y);
        mesh.positions_.push_back(v0.z);
        mesh.positions_.push_back(v1.x);
        mesh.positions_.push_back(v1.y);
        mesh.positions_.push_back(v1.z);

        // kolory
        for (int i = 0; i < 2; i++)
        {
            mesh.colors_.push_back(color.r);
            mesh.colors_.push_back(color.g);
            mesh.colors_.push_back(color.b);
        }
    }

    return mesh;
}
GraphicsApi::LineMesh MakeRayLineMesh(const glm::vec3& cameraPos, const glm::vec3& rayDir, float length = 100.0f,
                                      int segments = 10, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 0.0f))
{
    GraphicsApi::LineMesh mesh;

    glm::vec3 dir  = glm::normalize(rayDir);
    glm::vec3 step = dir * (length / segments);

    for (int i = 0; i < segments; ++i)
    {
        glm::vec3 p0 = cameraPos + step * float(i);
        glm::vec3 p1 = cameraPos + step * float(i + 1);

        mesh.positions_.insert(mesh.positions_.end(), {p0.x, p0.y, p0.z, p1.x, p1.y, p1.z});

        mesh.colors_.insert(mesh.colors_.end(), {color.r, color.g, color.b, color.r, color.g, color.b});
    }
    return mesh;
}
GraphicsApi::LineMesh appendLineMesh(const GraphicsApi::LineMesh& lineMesh1, const GraphicsApi::LineMesh& lineMesh2)
{
    GraphicsApi::LineMesh result = lineMesh1;
    result.positions_.insert(result.positions_.end(), lineMesh2.positions_.begin(), lineMesh2.positions_.end());
    result.colors_.insert(result.colors_.end(), lineMesh2.colors_.begin(), lineMesh2.colors_.end());
    return result;
}
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
    LOG_DEBUG << "perObjectBufferId";
    perObjectBufferId =
        graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate), GraphicsApi::DrawFlag::Dynamic);

    LOG_DEBUG << "perObjectBufferId";
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

DebugRenderer::DebugRenderer(RendererContext& rendererContext, Utils::Thread::IThreadSync& threadSync)
    : rendererContext_(rendererContext)
    , physicsVisualizator_(rendererContext.graphicsApi_, threadSync)
    , boundingBoxVisualizator_(rendererContext.graphicsApi_, threadSync)
    , rayVisualizator_(rendererContext.graphicsApi_, threadSync)
    , debugObjectShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::DebugObject)
    , gridShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::Grid)
    , debugNormalShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::DebugNormal)
    , textureShader_(rendererContext.graphicsApi_, GraphicsApi::ShaderProgramType::Texture)
{
}

DebugRenderer::~DebugRenderer()
{
    LOG_DEBUG << "";
    if (showPhycicsVisualizationSubId)
        EngineConf.debugParams.showPhycicsVisualization.unsubscribe(*showPhycicsVisualizationSubId);
}

void DebugRenderer::init()
{
    physicsVisualizator_.Init();
    boundingBoxVisualizator_.Init();
    rayVisualizator_.Init();
    debugObjectShader_.Init();
    gridShader_.Init();
    debugNormalShader_.Init();
    textureShader_.Init();

    boundingBoxVisualizator_.SetMeshCreationFunction(
        [&]() -> const GraphicsApi::LineMesh&
        {
            std::lock_guard<std::mutex> lk(meshInfoDebugObjectsMutex_);
            static GraphicsApi::LineMesh result;
            result.positions_.clear();
            result.colors_.clear();

            for (auto& [_, sub] : meshDebugInfoSubscribers_)
            {
                static const vec3 color(1, 0, 0);
                auto lineMesh = std::visit(
                    visitor{[](const Components::RendererComponent* rc)
                            {
                                if (not rc)
                                    return GraphicsApi::LineMesh{};
                                return CreateLineMeshFromBoundingBox(rc->getWorldSpaceBoundingBox(), color);
                            },
                            [](const Components::TreeRendererComponent* trc)
                            {
                                if (not trc)
                                    return GraphicsApi::LineMesh{};
                                return CreateLineMeshFromBoundingBox(trc->GetWorldBoundingBox(), color);
                            },
                            [](const Components::TerrainRendererComponent* tc)
                            {
                                if (tc and tc->GetRendererType() != Components::TerrainRendererComponent::RendererType::Mesh)
                                    return GraphicsApi::LineMesh{};

                                GraphicsApi::LineMesh result;
                                for (const auto& bb : tc->GetMeshTerrain()->getMeshesBoundingBoxes())
                                {
                                    result = appendLineMesh(result, CreateLineMeshFromBoundingBox(bb, color));
                                }
                                return result;
                            }},
                    sub.component);

                result = appendLineMesh(result, lineMesh);
            }
            return result;
        });

    rayVisualizator_.SetMeshCreationFunction(
        [&]() -> const GraphicsApi::LineMesh&
        {
            static GraphicsApi::LineMesh result;

            auto camera = rendererContext_.scene_->GetCameraManager().GetMainCamera();
            if (not camera)
            {
                return result;
            }
            auto ray =
                CalculateMouseRay(*camera, rendererContext_.scene_->getEngineContext()->GetInputManager().GetMousePosition());

            result = MakeRayLineMesh(camera->GetPosition(), ray.direction, 100.f, 1);
            LOG_DEBUG << "CamPos: " << camera->GetPosition() << " rayDir: " << ray.direction;
            return result;
        });

    gridPerObjectUpdateBufferId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(
        PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate), GraphicsApi::DrawFlag::Dynamic);

    if (gridPerObjectUpdateBufferId_)
    {
        PerObjectUpdate gridPerObjectUpdate;
        gridPerObjectUpdate.TransformationMatrix = Utils::CreateTransformationMatrix(vec3(0), DegreesVec3(-90, 0, 0), vec3(100));
        rendererContext_.graphicsApi_.UpdateShaderBuffer(*gridPerObjectUpdateBufferId_, &gridPerObjectUpdate);
    }
    else
    {
        LOG_ERROR << "gridPerObjectUpdateBufferId_ error!";
    }

    texturePerObjectUpdateBufferId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(
        PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate), GraphicsApi::DrawFlag::Dynamic);

    if (texturePerObjectUpdateBufferId_)
    {
        PerObjectUpdate gridPerObjectUpdate;
        gridPerObjectUpdate.TransformationMatrix = Utils::CreateTransformationMatrix(vec3(0), DegreesVec3(0), vec3(1));
        rendererContext_.graphicsApi_.UpdateShaderBuffer(*texturePerObjectUpdateBufferId_, &gridPerObjectUpdate);
    }
    else
    {
        LOG_ERROR << "texturePerObjectUpdateBufferId_ error!";
    }

    textureColorBufferId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(ColorBuffer),
                                                                             GraphicsApi::DrawFlag::Dynamic);
    if (textureColorBufferId_)
    {
        ColorBuffer b;
        b.color = vec4(1.f);
        rendererContext_.graphicsApi_.UpdateShaderBuffer(*textureColorBufferId_, &b);
    }

    meshDebugPerObjectBufferId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(
        PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate), GraphicsApi::DrawFlag::Dynamic);

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

    LOG_DEBUG << "Debug renderer initilized.";
}

void DebugRenderer::reloadShaders()
{
    debugObjectShader_.Reload();
    gridShader_.Reload();
    physicsVisualizator_.ReloadShader();
    boundingBoxVisualizator_.ReloadShader();
    rayVisualizator_.ReloadShader();
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
            case RenderState::BoundingBox:
                boundingBoxVisualizator_.Render();
                break;
            case RenderState::Ray:
                rayVisualizator_.Render();
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

    renderTextures(texturesToRender);
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
            meshDebugInfoSubscribers_.insert(
                {gameObject.GetId(),
                 {.gameObject = gameObject, .modelWrapper = terrainMeshComponent->GetModel(), .component = terrain}});
        }
    }
    auto rc = gameObject.GetComponent<Components::RendererComponent>();

    if (rc)
    {
        meshDebugInfoSubscribers_.insert(
            {gameObject.GetId(), {.gameObject = gameObject, .modelWrapper = rc->GetModelWrapper(), .component = rc}});
    }

    auto trc = gameObject.GetComponent<Components::TreeRendererComponent>();

    if (trc)
    {
        meshDebugInfoSubscribers_.insert(
            {gameObject.GetId(), {.gameObject = gameObject, .modelWrapper = trc->GetTrunkModel(), .component = trc}});
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
                    Utils::CreateTransformationMatrix(pos, size * vec2(-1, 1), DegreesFloat(180.f)));
                rendererContext_.graphicsApi_.UpdateShaderBuffer(*texturePerObjectUpdateBufferId_, &gridPerObjectUpdate);

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
    physicsVisualizator_.SetMeshCreationFunction(func);
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
                            debugInfoMesh.gameObject.GetWorldTransform().CalculateCurrentMatrix() * mesh.GetMeshTransform())};

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
void DebugRenderer::RemoveTextureToRender(GraphicsApi::ID id)
{
    auto iter = std::find(texturesToRender.begin(), texturesToRender.end(), id);
    if (iter != texturesToRender.end())
    {
        texturesToRender.erase(iter);
    }
}
void DebugRenderer::AddTextureToRender(GraphicsApi::ID id)
{
    texturesToRender.push_back(id);
}
}  // namespace GameEngine
