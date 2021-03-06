#include "TerrainRenderer.h"

#include <GLM/GLMUtils.h>
#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/Renderer/Terrain/TerrainTessellationRendererComponent.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
TerrainRenderer::TerrainRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Terrain)
    , clipPlane(vec4(0, 1, 0, 100000))
    , objectId(0)
    , perTerrainId(0)
    , perNodeId(0)
{
}

TerrainRenderer::~TerrainRenderer()
{
    if (objectId)
    {
        context_.graphicsApi_.DeleteObject(*objectId);
    }
}
void TerrainRenderer::init()
{
    shader_.Init();

    // clang-format off
    const std::vector<float> patches =
    {
        0, 0,
        0.333f, 0,
        0.666f, 0,
        1, 0,

        0, 0.333f,
        0.333f, 0.333f,
        0.666f, 0.333f,
        1, 0.333f,

        0, 0.666f,
        0.333f, 0.666f,
        0.666f, 0.666f,
        1, 0.666f,

        0, 1,
        0.333f, 1,
        0.666f, 1,
        1, 1
    };
    // clang-format on

    objectId = context_.graphicsApi_.CreatePatchMesh(patches);

    perTerrainId = context_.graphicsApi_.CreateShaderBuffer(PER_TERRAIN_BIND_LOCATION, sizeof(PerTerrain));
    if (not perTerrainId)
    {
        return;
    }

    perNodeId = context_.graphicsApi_.CreateShaderBuffer(PER_NODE_LOCATION, sizeof(PerNode));
    if (not perNodeId)
    {
        return;
    }
}

void TerrainRenderer::render()
{
    if (not IsInit() or subscribes_.empty())
        return;

    context_.graphicsApi_.DisableCulling();
    shader_.Start();

    RenderSubscribers();
}
void TerrainRenderer::RenderSubscribers() const
{
    for (auto& sub : subscribes_)
    {
        const auto& tree   = sub.second->GetTree();
        const auto& config = sub.second->GetConfiguration();

        context_.graphicsApi_.UpdateShaderBuffer(*perTerrainId, &config.GetPerTerrainBuffer());
        context_.graphicsApi_.BindShaderBuffer(*perTerrainId);

        BindTextures(sub.second->GetTextures());

        for (const auto& node : tree.GetNodes())
        {
            RenderNode(*node);
        }
    }
}
void TerrainRenderer::RenderSubscriber(const std::vector<std::pair<TerrainTextureType, Texture*>>& textures) const
{
    BindTextures(textures);
    context_.graphicsApi_.RenderPurePatchedMeshInstances(*objectId);
}

void TerrainRenderer::RenderNode(const TerrainNode& node) const
{
    if (node.IsLeaf())
    {
        context_.graphicsApi_.UpdateShaderBuffer(*perNodeId, &node.GetPerNodeBuffer());
        context_.graphicsApi_.BindShaderBuffer(*perNodeId);
        context_.graphicsApi_.RenderMesh(*objectId);
    }

    for (const auto& node : node.GetChildren())
    {
        if (node)
        {
            RenderNode(*node);
        }
    }
}
void TerrainRenderer::BindTextures(const std::vector<std::pair<TerrainTextureType, Texture*>>& textures) const
{
    for (const auto& t : textures)
    {
        auto texture = t.second;
        if (texture and texture->GetGraphicsObjectId())
        {
            BindTexture(*texture, static_cast<uint32>(t.first));
        }
    }
}
bool TerrainRenderer::IsInit() const
{
    return shader_.IsReady() and objectId.has_value() and perTerrainId.has_value() and perNodeId.has_value();
}
void TerrainRenderer::BindTexture(Texture& texture, uint32 id) const
{
    context_.graphicsApi_.ActiveTexture(id, *texture.GetGraphicsObjectId());
}
void TerrainRenderer::subscribe(GameObject& gameObject)
{
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                     [&gameObject](const auto& obj) { return obj.first == gameObject.GetId(); });

    if (iter == subscribes_.end())
    {
        auto terrain = gameObject.GetComponent<Components::TerrainRendererComponent>();

        if (not terrain or terrain->GetRendererType() != Components::TerrainRendererComponent::RendererType::Tessellation)
            return;

        subscribes_.push_back({gameObject.GetId(), terrain->GetTesselationTerrain()});
    }
}
void TerrainRenderer::unSubscribe(GameObject& gameObject)
{
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [&gameObject](const auto& obj) { return obj.first == gameObject.GetId(); });

    if (iter != subscribes_.end())
    {
        subscribes_.erase(iter);
    }
}
void TerrainRenderer::unSubscribeAll()
{
    subscribes_.clear();
}
void TerrainRenderer::reloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
