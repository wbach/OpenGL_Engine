#include "TerrainMeshRenderer.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
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
TerrainMeshRenderer::TerrainMeshRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TerrainMesh)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainMeshRenderer::Render);
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
    if (subscribes_.empty())
        return;
    context_.graphicsApi_.EnableCulling();
    shader_.Start();
    RenderSubscribers(scene);
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
        context_.graphicsApi_.BindShaderBuffer(*subscriber.component_->GetPerObjectUpdateBuffer(index++));
        RenderMesh(mesh);
    }
}
void TerrainMeshRenderer::RenderMesh(const Mesh& mesh) const
{
    context_.graphicsApi_.RenderTriangleStripMesh(mesh.GetGraphicsObjectId());
}
void TerrainMeshRenderer::PartialRendering(const Scene&, const Subscriber& subscriber) const
{
    const auto& model = subscriber.component_->GetModel().Get(LevelOfDetail::L1);

    if (not model)
        return;

    const auto& terrainPosition = subscriber.gameObject_->worldTransform.GetPosition();
    const auto& config          = subscriber.component_->GetConfiguration();

    auto partsCount = *config.GetPartsCount();
    auto scaleXZ    = config.GetScaleXZ();
    auto partScale  = scaleXZ / partsCount;

    auto firstIndex = -static_cast<float>(partsCount) / 2.f * partScale + partScale / 2.f;
    auto position   = terrainPosition + vec3(firstIndex, 0, firstIndex);

    uint32 index = 0;
    uint32 rendererdTerrains = 0;
    for (const auto& mesh : model->GetMeshes())
    {
        auto x = index % partsCount;
        auto z = index / partsCount;
        auto centerPosition = position + vec3(x * partScale, 0, z * partScale);

        auto isVisible =
          context_.frustrum_.SphereIntersection(centerPosition, (partScale * 1.41 ) / 2.f);

        if (isVisible)
        {
            context_.graphicsApi_.BindShaderBuffer(*subscriber.component_->GetPerObjectUpdateBuffer(index));
            RenderMesh(mesh);
            ++rendererdTerrains;
        }
        ++index;
    }
   // DEBUG_LOG("Terrains count :" + std::to_string(rendererdTerrains));
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
