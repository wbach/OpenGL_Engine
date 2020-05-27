#include "TerrainMeshRenderer.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Objects/GameObject.h"
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
    measurementValue_ = &context.measurmentHandler_.AddNewMeasurment(TERRAIN_MEASURMENT_NAME, "0");
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
    context_.graphicsApi_.EnableBlend();
    shader_.Start();
    std::lock_guard<std::mutex> lk(subscriberMutex_);
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
    const auto& model = subscriber.component_->GetModel().Get(LevelOfDetail::L1);

    if (not model or not subscriber.component_)
        return;

    auto isVisible = context_.frustrum_.intersection(model->getBoundingBox());

    if (isVisible)
    {
        BindTextures(subscriber.component_->GetTextures());

        if (subscriber.component_->GetConfiguration().GetPartsCount())
        {
            PartialRendering(*model, scene, *subscriber.component_);
        }
        else
        {
            uint32 index = 0;
            for (const auto& mesh : model->GetMeshes())
            {
                if (mesh.GetGraphicsObjectId())
                    RenderMesh(mesh, subscriber.component_->GetPerObjectUpdateBuffer(index++));
            }
        }
    }
}
void TerrainMeshRenderer::RenderMesh(const Mesh& mesh, const GraphicsApi::ID& bufferId) const
{
    if (bufferId)
    {
        context_.graphicsApi_.BindShaderBuffer(*bufferId);
        context_.graphicsApi_.RenderTriangleStripMesh(*mesh.GetGraphicsObjectId());
        ++renderedTerrains;
    }
}
void TerrainMeshRenderer::PartialRendering(const Model& model, const Scene&,
                                           const Components::TerrainMeshRendererComponent& component) const
{
    uint32 index = 0;

    for (const auto& mesh : model.GetMeshes())
    {
        auto isVisible = context_.frustrum_.intersection(mesh.getBoundingBox());
        if (isVisible)
        {
            RenderMesh(mesh, component.GetPerObjectUpdateBuffer(index));
        }
        ++index;
    }
}
void TerrainMeshRenderer::BindTextures(const TerrainTexturesMap& textures) const
{
    //    auto shadowMap = context_.shadowsFrameBuffer_.GetShadowMap();

    //    if (shadowMap)
    //    {
    //        context_.graphicsApi_.ActiveTexture(0, *shadowMap);
    //    }

    for (const auto& t : textures)
    {
        auto texture = t.second;

        if (texture and texture->GetGraphicsObjectId())
        {
            BindTexture(texture, static_cast<uint32>(t.first));
        }
    }
}
void TerrainMeshRenderer::BindTexture(Texture* texture, uint32 id) const
{
    context_.graphicsApi_.ActiveTexture(id, *texture->GetGraphicsObjectId());
}
void TerrainMeshRenderer::Subscribe(GameObject* gameObject)
{
    auto terrain = gameObject->GetComponent<Components::TerrainRendererComponent>();

    if (not terrain or terrain->GetRendererType() != Components::TerrainRendererComponent::RendererType::Mesh)
        return;

    DEBUG_LOG("Subscribe goId : " + std::to_string(gameObject->GetId()));
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    subscribes_.push_back({gameObject->GetId(), {gameObject, terrain->GetMeshTerrain()}});
}
void TerrainMeshRenderer::UnSubscribe(GameObject* gameObject)
{
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [gameObject](const auto& obj) { return obj.first == gameObject->GetId(); });

    if (iter != subscribes_.end())
    {
        std::lock_guard<std::mutex> lk(subscriberMutex_);
        subscribes_.erase(iter);
    }
}
void TerrainMeshRenderer::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
