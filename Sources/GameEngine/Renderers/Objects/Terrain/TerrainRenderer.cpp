#include "TerrainRenderer.h"
#include <algorithm>
#include "GLM/GLMUtils.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Objects/Shadows/ShadowFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainTessellationRendererComponent.h"
#include <Logger/Log.h>

namespace GameEngine
{

TerrainRenderer::TerrainRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_,GraphicsApi::ShaderProgramType::Terrain)
    , clipPlane(vec4(0, 1, 0, 100000))
    , objectId(0)
    , perTerrainId(0)
    , perNodeId(0)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainRenderer::Render);
}
void TerrainRenderer::Init()
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

void TerrainRenderer::Render(const Scene& scene, const Time&)
{
    if (not IsInit() or subscribes_.empty())
        return;

    context_.graphicsApi_.DisableCulling();
    shader_.Start();

    auto modelViewMatrix = scene.GetCamera().GetViewMatrix();
    RenderSubscribers(modelViewMatrix);
}
void TerrainRenderer::RenderSubscribers(const mat4& viewMatrix) const
{
    for (auto& sub : subscribes_)
    {
        const auto& tree   = sub.second->GetTree();
        const auto& config = sub.second->GetConfig();
        DEBUG_LOG(std::to_string(config.GetPerTerrainBuffer().scale.value));
        context_.graphicsApi_.UpdateShaderBuffer(*perTerrainId, &config.GetPerTerrainBuffer());
        context_.graphicsApi_.BindShaderBuffer(*perTerrainId);

        BindTextures(sub.second->GetTextures());

        for (const auto& node : tree.GetNodes())
        {
            RenderNode(*node);
        }
    }
}
void TerrainRenderer::RenderSubscriber(const TerrainTexturesMap& textures) const
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
void TerrainRenderer::BindTextures(const TerrainTexturesMap& textures) const
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
bool TerrainRenderer::IsInit() const
{
    return shader_.IsReady() and objectId.has_value() and perTerrainId.has_value() and perNodeId.has_value();
}
void TerrainRenderer::BindTexture(Texture* texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_.ActiveTexture(id, texture->GetGraphicsObjectId());
}
void TerrainRenderer::Subscribe(GameObject* gameObject)
{
    auto terrain = gameObject->GetComponent<Components::TerrainRendererComponent>();

    if (not terrain or terrain->GetRendererType() != Components::TerrainRendererComponent::RendererType::Tessellation)
        return;

    subscribes_.push_back({gameObject->GetId(), terrain->GetTesselationTerrain()});
}
void TerrainRenderer::UnSubscribe(GameObject* gameObject)
{
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [gameObject](const auto& obj) { return obj.first == gameObject->GetId(); });

    if (iter != subscribes_.end())
    {
        subscribes_.erase(iter);
    }
}
void TerrainRenderer::UnSubscribeAll()
{
    subscribes_.clear();
}
void TerrainRenderer::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
