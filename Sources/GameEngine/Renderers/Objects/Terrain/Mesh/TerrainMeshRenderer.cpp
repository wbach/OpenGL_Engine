#include "TerrainMeshRenderer.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Objects/Shadows/ShadowFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
namespace
{
uint32 renderedTerrains = 0;
const std::string TERRAIN_MEASURMENT_NAME{"TerrainMeshesRendered"};
}  // namespace
TerrainMeshRenderer::TerrainMeshRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TerrainMesh)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainMeshRenderer::Render);
    measurementValue_ = &EngineContext.AddNewMeasurment(TERRAIN_MEASURMENT_NAME, "0");
}
TerrainMeshRenderer::~TerrainMeshRenderer()
{
}
void TerrainMeshRenderer::Init()
{
    shader_.Init();
}
void TerrainMeshRenderer::Render(const Scene& scene, const Time&)
{
    renderedTerrains = 0;

    if (subscribes_.empty())
        return;
    context_.graphicsApi_.EnableCulling();
    shader_.Start();
    RenderSubscribers(scene);

    *measurementValue_ = std::to_string(renderedTerrains);
}
void TerrainMeshRenderer::RenderSubscribers(const Scene& scene) const
{
    for (const auto& sub : subscribes_)
    {
        RenderSubscriber(scene, sub.second);
    }
}

void TerrainMeshRenderer::RenderSubscriber(const Scene& scene, const Subscriber& subscriber) const
{
    BindTextures(subscriber.component_->GetTextures());

    if (subscriber.component_->GetConfiguration().GetPartsCount())
    {
        PartialRendering(scene, subscriber);
        return;
    }

    const auto& model = subscriber.component_->GetModel().Get(LevelOfDetail::L1);

    if (not model)
        return;

    int index = 0;
    for (const auto& mesh : model->GetMeshes())
    {
        RenderMesh(mesh, subscriber.component_->GetPerObjectUpdateBuffer(index++));
    }
}
void TerrainMeshRenderer::RenderMesh(const Mesh& mesh, const GraphicsApi::ID& id) const
{
    context_.graphicsApi_.BindShaderBuffer(*id);
    context_.graphicsApi_.RenderTriangleStripMesh(mesh.GetGraphicsObjectId());
    ++renderedTerrains;
}
void TerrainMeshRenderer::PartialRendering(const Scene&, const Subscriber& subscriber) const
{
    const auto& model = subscriber.component_->GetModel().Get(LevelOfDetail::L1);

    if (not model)
        return;

    const auto& terrainPosition = subscriber.gameObject_->GetWorldTransform().GetPosition();
    const auto& config          = subscriber.component_->GetConfiguration();

    auto partsCount = *config.GetPartsCount();
    auto scaleXZ    = config.GetScaleXZ();
    auto partScale  = scaleXZ / partsCount;

    auto firstIndex = -static_cast<float>(partsCount) / 2.f * partScale + partScale / 2.f;
    auto position   = terrainPosition + vec3(firstIndex, 0, firstIndex);

    uint32 index = 0;

    for (const auto& mesh : model->GetMeshes())
    {
        auto x              = index % partsCount;
        auto z              = index / partsCount;
        auto centerPosition = position + vec3(x * partScale, 0, z * partScale);

        auto isVisible = context_.frustrum_.SphereIntersection(centerPosition, (partScale * 1.41f) / 2.f);

        if (isVisible)
        {
            RenderMesh(mesh, subscriber.component_->GetPerObjectUpdateBuffer(index));
        }
        ++index;
    }
}
void TerrainMeshRenderer::BindTextures(const TerrainTexturesMap& textures) const
{
    auto shadowMap = context_.shadowsFrameBuffer_.GetShadowMap();

    if (shadowMap)
    {
        context_.graphicsApi_.ActiveTexture(0, *shadowMap);
    }

    for (const auto& t : textures)
    {
        if (t.second->IsLoadedToGpu())
        {
            BindTexture(t.second, static_cast<int>(t.first));
        }
    }
}
void TerrainMeshRenderer::BindTexture(Texture* texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_.ActiveTexture(id, texture->GetGraphicsObjectId());
}
void TerrainMeshRenderer::Subscribe(GameObject* gameObject)
{
    auto terrain = gameObject->GetComponent<Components::TerrainRendererComponent>();

    if (not terrain or terrain->GetRendererType() != Components::TerrainRendererComponent::RendererType::Mesh)
        return;

    DEBUG_LOG("Subscribe goId : " + std::to_string(gameObject->GetId()));
    subscribes_.push_back({gameObject->GetId(), {gameObject, terrain->GetMeshTerrain()}});
}
void TerrainMeshRenderer::UnSubscribe(GameObject* gameObject)
{
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [gameObject](const auto& obj) { return obj.first == gameObject->GetId(); });

    if (iter != subscribes_.end())
    {
        subscribes_.erase(iter);
    }
}
void TerrainMeshRenderer::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
