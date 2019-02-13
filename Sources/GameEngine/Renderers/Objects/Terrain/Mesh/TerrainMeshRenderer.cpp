#include "TerrainMeshRenderer.h"
#include <algorithm>
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainDef.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Objects/Shadows/ShadowFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "Logger/Log.h"
#include "Shaders/TerrainMeshShaderUniforms.h"

namespace GameEngine
{
TerrainMeshRenderer::TerrainMeshRenderer(RendererContext& context)
    : context_(context)
    , clipPlane(vec4(0, 1, 0, 100000))
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::TerrainMesh);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainMeshRenderer::Render);
}
TerrainMeshRenderer::~TerrainMeshRenderer()
{
}
void TerrainMeshRenderer::Init()
{
    InitShader();
}
void TerrainMeshRenderer::InitShader()
{
    shader_->Init();
    shader_->Start();
    // shader_->Load(TerrainMeshShaderUniforms::projectionMatrix, context_.projection_.GetProjectionMatrix());
    // shader_->Load(TerrainMeshShaderUniforms::useShadows, EngineConf.renderer.shadows.isEnabled);
    // shader_->Load(TerrainMeshShaderUniforms::shadowDistance, EngineConf.renderer.shadows.distance);
    // shader_->Load(TerrainMeshShaderUniforms::shadowMapSize, EngineConf.renderer.shadows.mapSize);
    // shader_->Load(TerrainMeshShaderUniforms::useNormalMap, true);

    shader_->Stop();
}
void TerrainMeshRenderer::Render(Scene* scene)
{
    if (subscribes_.empty())
        return;
    context_.graphicsApi_.EnableCulling();
    shader_->Start();
    // shader_->Load(TerrainMeshShaderUniforms::toShadowMapSpace, context_.toShadowMapZeroMatrix_);
    // shader_->Load(TerrainMeshShaderUniforms::viewMatrix, scene->GetCamera()->GetViewMatrix());
    RenderSubscribers();
}
void TerrainMeshRenderer::RenderSubscribers() const
{
    for (const auto& sub : subscribes_)
    {
        RenderSubscriber(sub.second);
    }
}
void TerrainMeshRenderer::RenderSubscriber(const Subscriber& subscriber) const
{
    BindTextures(subscriber.component_->GetTextures());

    const auto& model = subscriber.component_->GetModel().Get(LevelOfDetail::L1);
    const auto& perUpdate = subscriber.component_->GetPerObjectUpdateBuffers();
    if (not model)
        return;

    int index = 0;
    for (const auto& mesh : model->GetMeshes())
    {
        context_.graphicsApi_.BindShaderBuffer(*perUpdate[index++].GetId());
        RenderMesh(mesh);
    }
}
void TerrainMeshRenderer::RenderModel(const Model& model) const
{
    for (const auto& mesh : model.GetMeshes())
    {
        RenderMesh(mesh);
    }
}
void TerrainMeshRenderer::RenderMesh(const Mesh& mesh) const
{
    context_.graphicsApi_.RenderTriangleStripMesh(mesh.GetObjectId());
}
void TerrainMeshRenderer::BindTextures(const TerrainTexturesMap& textures) const
{
    context_.graphicsApi_.ActiveTexture(0, context_.shadowsFrameBuffer_.GetShadowMap());

    for (const auto& t : textures)
        BindTexture(t.second, static_cast<int>(t.first));
}
void TerrainMeshRenderer::BindTexture(Texture* texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_.ActiveTexture(id, texture->GetId());
}
void TerrainMeshRenderer::Subscribe(GameObject* gameObject)
{
    auto terrain = gameObject->GetComponent<Components::TerrainMeshRendererComponent>();

    if (terrain == nullptr)
        return;

    subscribes_.push_back({gameObject->GetId(), {gameObject, terrain}});
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
    shader_->Reload();
    InitShader();
}
}  // namespace GameEngine
