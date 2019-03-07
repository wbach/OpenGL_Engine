#include "TerrainRenderer.h"
#include <algorithm>
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainDef.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Objects/Shadows/ShadowFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "Logger/Log.h"
#include "Shaders/TerrainShaderUniforms.h"

namespace GameEngine
{
TerrainRenderer::TerrainRenderer(RendererContext& context)
    : context_(context)
    , clipPlane(vec4(0, 1, 0, 100000))
    , objectId(0)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::Terrain);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainRenderer::Render);
}
void TerrainRenderer::Init()
{
    InitShader();
    // objectId = context_.graphicsApi_.CreatePurePatchMeshInstanced(
    //    4, static_cast<uint32>(TerrainDef::SIZE * TerrainDef::SIZE));

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
}

void TerrainRenderer::Render(const Scene& scene, const Time&)
{
    if (subscribes_.empty())
        return;

    shader_->Start();
    shader_->Load(TerrainShaderUniforms::cameraPosition, scene.GetCamera().GetPosition());

    auto modelViewMatrix  = scene.GetCamera().GetViewMatrix();
    RenderSubscribers(modelViewMatrix);
}
void TerrainRenderer::RenderSubscribers(const mat4& viewMatrix) const
{
    for (auto& sub : subscribes_)
    {
        const auto& tree = sub.second->GetTree();
        const auto& config = sub.second->GetConfig();
        shader_->Load(TerrainShaderUniforms::m_ViewProjection, context_.projection_.GetProjectionMatrix() * viewMatrix);
        shader_->Load(TerrainShaderUniforms::scaleY, config.GetScaleY());
        shader_->Load(TerrainShaderUniforms::lod_morph_area_1, config.GetMorphingArea(0));
        shader_->Load(TerrainShaderUniforms::lod_morph_area_2, config.GetMorphingArea(1));
        shader_->Load(TerrainShaderUniforms::lod_morph_area_3, config.GetMorphingArea(2));
        shader_->Load(TerrainShaderUniforms::lod_morph_area_4, config.GetMorphingArea(3));
        shader_->Load(TerrainShaderUniforms::lod_morph_area_5, config.GetMorphingArea(4));
        shader_->Load(TerrainShaderUniforms::lod_morph_area_6, config.GetMorphingArea(5));
        shader_->Load(TerrainShaderUniforms::lod_morph_area_7, config.GetMorphingArea(6));
        shader_->Load(TerrainShaderUniforms::lod_morph_area_8, config.GetMorphingArea(7));

        for (const auto& node : tree.GetNodes())
        {
            auto gap = node->GetGap();
            shader_->Load(TerrainShaderUniforms::gap, gap);
            RenderNode(*node);
        }
    }
}
void TerrainRenderer::RenderSubscriber(const TerrainTexturesMap& textures) const
{
    BindTextures(textures);
    context_.graphicsApi_.RenderPurePatchedMeshInstances(objectId);
}
void TerrainRenderer::InitShader()
{
    shader_->Init();
}
void TerrainRenderer::RenderNode(const TerrainNode& node) const
{
    if (node.IsLeaf())
    {
        shader_->Load(TerrainShaderUniforms::lod, node.GetLod());
        shader_->Load(TerrainShaderUniforms::index, node.GetIndex());
        shader_->Load(TerrainShaderUniforms::location, node.GetLocation());
        shader_->Load(TerrainShaderUniforms::worldMatrix, node.GetWorldMatrix());
        shader_->Load(TerrainShaderUniforms::localMatrix, node.GetLocalMatrix());
        context_.graphicsApi_.RenderMesh(objectId);
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
    context_.graphicsApi_.ActiveTexture(0, context_.shadowsFrameBuffer_.GetShadowMap());

    for (const auto& t : textures)
        BindTexture(t.second, static_cast<int>(t.first));
}
void TerrainRenderer::BindTexture(Texture* texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_.ActiveTexture(id, texture->GetId());
}
void TerrainRenderer::Subscribe(GameObject* gameObject)
{
    auto terrain = gameObject->GetComponent<Components::TerrainRendererComponent>();

    if (terrain == nullptr)
        return;

    subscribes_.push_back({gameObject->GetId(), terrain});
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
void TerrainRenderer::ReloadShaders()
{
    shader_->Reload();
    InitShader();
}
}  // namespace GameEngine
