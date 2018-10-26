#include "TerrainRenderer.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Engine/EngineMeasurement.h"
#include "GameEngine/Objects/RenderAble/Terrain/Terrain.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainDef.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainWrapper.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Objects/Shadows/ShadowFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Utils/Time/Timer.h"

namespace GameEngine
{
TerrainRenderer::TerrainRenderer(RendererContext& context)
    : context_(context)
    , shader(context.graphicsApi_)
    , clipPlane(vec4(0, 1, 0, 100000))
    , objectId(0)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainRenderer::Render);
}
void TerrainRenderer::Init()
{
    InitShader();
    objectId = context_.graphicsApi_->CreatePurePatchMeshInstanced(
        4, static_cast<uint32>(TerrainDef::SIZE * TerrainDef::SIZE));
    Log("TerrainRenderer initialized.");
}

void TerrainRenderer::Render(Scene* scene)
{
    Utils::Timer timer;
    context_.defferedFrameBuffer_->BindToDraw();

    shader.Start();
    shader.Load(TerrainShader::lightDirection, scene->GetDirectionalLight().GetDirection());
    shader.Load(TerrainShader::playerPosition, scene->GetCamera()->GetPosition());
    shader.Load(TerrainShader::toShadowMapSpace, context_.toShadowMapZeroMatrix_);

    auto modelViewMatrix  = scene->GetCamera()->GetViewMatrix();
    modelViewMatrix[3][0] = 0;
    modelViewMatrix[3][1] = 0;
    modelViewMatrix[3][2] = 0;

    RenderSubscribers(modelViewMatrix, 2);
    shader.Stop();

    MakeMeasurement("TerrainRenderer", timer.GetTimeMiliseconds());
}
void TerrainRenderer::RenderSubscribers(const mat4& viewMatrix, int range) const
{
    for (auto& sub : subscribes)
    {
        shader.Load(TerrainShader::modelViewMatrix, viewMatrix);
        shader.Load(TerrainShader::modelViewProjectionMatrix, context_.projection_->GetProjectionMatrix() * viewMatrix);
        RenderSubscriber(sub);
    }
}
void TerrainRenderer::RenderSubscriber(TerrainPtr sub) const
{
    if (sub == nullptr)
        return;

    BindTextures(sub);
    context_.graphicsApi_->RenderPurePatchedMeshInstances(objectId);
}
void TerrainRenderer::InitShader()
{
    shader.Init();
    shader.Start();
    shader.Load(TerrainShader::heightFactor, TerrainDef::HEIGHT_FACTOR);
    shader.Load(TerrainShader::shadowVariables, vec3(1.f, 35.f, 2048.f));
    shader.Load(TerrainShader::projectionMatrix, context_.projection_->GetProjectionMatrix());
    shader.Stop();
}
void TerrainRenderer::BindTextures(TerrainPtr terrain) const
{
    context_.graphicsApi_->ActiveTexture(0, context_.shadowsFrameBuffer_->GetShadowMap());

    const auto& textures = terrain->Get()->textures;
    for (const auto& t : textures)
        BindTexture(t.second, static_cast<int>(t.first));
}
void TerrainRenderer::BindTexture(Texture *texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_->ActiveTexture(id, texture->GetId());
}
void TerrainRenderer::Subscribe(GameObject* gameObject)
{
    auto terrain = dynamic_cast<TerrainPtr>(gameObject);
    if (terrain == nullptr)
        return;

    subscribes.push_back(terrain);
}
void TerrainRenderer::UnSubscribe(GameObject* gameObject)
{
    for (auto iter = subscribes.begin(); iter != subscribes.end(); ++iter)
    {
        if ((*iter)->GetId() == gameObject->GetId())
        {
            subscribes.erase(iter);
            return;
        }
    }
}
void TerrainRenderer::ReloadShaders()
{
    shader.Reload();
    InitShader();
}
}
