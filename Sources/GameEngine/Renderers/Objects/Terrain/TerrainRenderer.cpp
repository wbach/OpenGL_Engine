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
    shader_ = context.shaderFactory_.create(Shaders::Terrain);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainRenderer::Render);
}
void TerrainRenderer::Init()
{
    InitShader();
    objectId = context_.graphicsApi_->CreatePurePatchMeshInstanced(
        4, static_cast<uint32>(TerrainDef::SIZE * TerrainDef::SIZE));
}

void TerrainRenderer::Render(Scene* scene)
{
    shader_->Start();
    shader_->Load(TerrainShaderUniforms::lightDirection, scene->GetDirectionalLight().GetDirection());
    shader_->Load(TerrainShaderUniforms::playerPosition, scene->GetCamera()->GetPosition());
    shader_->Load(TerrainShaderUniforms::toShadowMapSpace, context_.toShadowMapZeroMatrix_);

    auto modelViewMatrix  = scene->GetCamera()->GetViewMatrix();
    modelViewMatrix[3][0] = 0;
    modelViewMatrix[3][1] = 0;
    modelViewMatrix[3][2] = 0;

    RenderSubscribers(modelViewMatrix, 2);
}
void TerrainRenderer::RenderSubscribers(const mat4& viewMatrix, int range) const
{
    for (auto& sub : subscribes_)
    {
        shader_->Load(TerrainShaderUniforms::modelViewMatrix, viewMatrix);
        shader_->Load(TerrainShaderUniforms::modelViewProjectionMatrix,
                      context_.projection_->GetProjectionMatrix() * viewMatrix);
        RenderSubscriber(sub.second->GetTextures());
    }
}
void TerrainRenderer::RenderSubscriber(const TerrainTexturesMap& textures) const
{
    BindTextures(textures);
    context_.graphicsApi_->RenderPurePatchedMeshInstances(objectId);
}
void TerrainRenderer::InitShader()
{
    shader_->Init();
    shader_->Start();
    shader_->Load(TerrainShaderUniforms::heightFactor, TerrainDef::HEIGHT_FACTOR);
    shader_->Load(TerrainShaderUniforms::shadowVariables, vec3(1.f, 35.f, 2048.f));
    shader_->Load(TerrainShaderUniforms::projectionMatrix, context_.projection_->GetProjectionMatrix());
    shader_->Stop();
}
void TerrainRenderer::BindTextures(const TerrainTexturesMap& textures) const
{
    context_.graphicsApi_->ActiveTexture(0, context_.shadowsFrameBuffer_->GetShadowMap());

    for (const auto& t : textures)
        BindTexture(t.second, static_cast<int>(t.first));
}
void TerrainRenderer::BindTexture(Texture* texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_->ActiveTexture(id, texture->GetId());
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
