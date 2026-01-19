#include "TerrainMeshRenderer.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
TerrainMeshRenderer::TerrainMeshRenderer(RendererContext& context)
    : context_(context)
{
}
TerrainMeshRenderer::~TerrainMeshRenderer()
{
}
uint32 TerrainMeshRenderer::renderSubscribers()
{
    renderedTerrains_ = 0;
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    for (const auto& sub : subscribes_)
    {
        renderSubscriber(sub.second);
    }
    return renderedTerrains_;
}

void TerrainMeshRenderer::renderSubscriber(const Subscriber& subscriber)
{
    const auto& model = subscriber.component_->GetModel().Get(LevelOfDetail::L1);

    if (not model or not subscriber.component_)
    {
        LOG_DEBUG << "not model or not subscriber.component_";
        return;
    }

    auto isVisible = context_.frustrum_.intersection(subscriber.component_->getModelBoundingBox());

    if (isVisible)
    {
        auto textureBufferId = subscriber.component_->getPerTerrainTexturesBufferId();
        if (textureBufferId)
        {
            context_.graphicsApi_.BindShaderBuffer(*textureBufferId);
        }
        bindTextures(subscriber.component_->GetTextures());
        renderTerrainMeshes(*model, *subscriber.component_);
    }
}
void TerrainMeshRenderer::renderMesh(const Mesh& mesh, const GraphicsApi::ID& bufferId)
{
    if (bufferId)
    {
        context_.graphicsApi_.BindShaderBuffer(*bufferId);
        context_.graphicsApi_.RenderTriangleStripMesh(*mesh.GetGraphicsObjectId());
        ++renderedTerrains_;
    }
}
void TerrainMeshRenderer::renderTerrainMeshes(const Model& model, const Components::TerrainMeshRendererComponent& component)
{
    uint32 index = 0;
    for (const auto& mesh : model.GetMeshes())
    {
        auto isVisible = context_.frustrum_.intersection(component.getMeshBoundingBox(index));
        if (isVisible and mesh.GetGraphicsObjectId())
        {
            renderMesh(mesh, component.GetPerObjectUpdateBuffer(index));
        }
        ++index;
    }
}
void TerrainMeshRenderer::bindTextures(const std::vector<std::pair<TerrainTextureType, Texture*>>& textures) const
{
    Texture* textureMap[21] = {nullptr};

    for (const auto& [type, texture] : textures)
    {
        uint32 index = static_cast<uint32>(type);
        if (index < 21)
        {
            textureMap[index] = texture;
        }
    }

    auto allTypes = magic_enum::enum_values<TerrainTextureType>();

    for (const auto& type : allTypes)
    {
        uint32 slot  = static_cast<uint32>(type);
        Texture* tex = textureMap[slot];

        if (tex && tex->GetGraphicsObjectId() != 0)
        {
            bindTexture(tex, slot);
        }
        else
        {
            context_.graphicsApi_.ActiveTexture(slot, 0);
        }
    }
}
void TerrainMeshRenderer::bindTexture(Texture* texture, uint32 id) const
{
    context_.graphicsApi_.ActiveTexture(id, *texture->GetGraphicsObjectId());
}
void TerrainMeshRenderer::subscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [&gameObject](const auto& obj) { return obj.first == gameObject.GetId(); });

    if (iter == subscribes_.end())
    {
        auto terrain = gameObject.GetComponent<Components::TerrainRendererComponent>();

        if (not terrain or terrain->GetRendererType() != Components::TerrainRendererComponent::RendererType::Mesh)
            return;

        LOG_DEBUG << "Subscribe goId : " << gameObject.GetId();
        auto terrainMeshComponent = terrain->GetMeshTerrain();
        if (terrainMeshComponent)
        {
            subscribes_.push_back({gameObject.GetId(), {&gameObject, terrainMeshComponent}});
        }
        else
        {
            LOG_ERROR << "terrainMeshComponent not exist";
        }
    }
}
void TerrainMeshRenderer::unSubscribe(GameObject& gameObject)
{
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [&gameObject](const auto& obj) { return obj.first == gameObject.GetId(); });

    if (iter != subscribes_.end())
    {
        LOG_DEBUG << "unSubscribe goId : " << gameObject.GetId();
        std::lock_guard<std::mutex> lk(subscriberMutex_);
        subscribes_.erase(iter);
    }
}
void TerrainMeshRenderer::unSubscribeAll()
{
    LOG_DEBUG << "";
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    subscribes_.clear();
}
}  // namespace GameEngine
