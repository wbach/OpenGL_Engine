#include "DebugRenderer.h"
#include <Logger/Log.h>
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Renderers/Projection.h"

GameEngine::DebugRenderer::DebugRenderer(const RendererContext& context)
    : context_(context)
    , simpleShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::SimpleEntity)
    , gridShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Grid)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, DebugRenderer::Render);
}

GameEngine::DebugRenderer::~DebugRenderer()
{
}

void GameEngine::DebugRenderer::Init()
{
    simpleShader_.Init();
    gridShader_.Init();
}

void GameEngine::DebugRenderer::ReloadShaders()
{
    simpleShader_.Reload();
    gridShader_.Reload();
}

void GameEngine::DebugRenderer::Render(const Scene& scene, const Time& threadTime)
{
    DrawGrid();
    DrawDebugObjects();

    if (physicsDebugDraw_)
    {
        DEBUG_LOG("Physic draw");
        physicsDebugDraw_(scene.GetCamera().GetViewMatrix(), context_.projection_.GetProjectionMatrix());
    }
}

void GameEngine::DebugRenderer::SetPhysicsDebugDraw(std::function<void(const mat4&, const mat4&)> func)
{
    physicsDebugDraw_ = func;
}

void GameEngine::DebugRenderer::InitShaders()
{
}

void GameEngine::DebugRenderer::DrawGrid()
{
    if (not gridShader_.IsReady())
        return;

    gridShader_.Start();
    context_.graphicsApi_.RenderQuad();
    gridShader_.Stop();
}

void GameEngine::DebugRenderer::DrawDebugObjects()
{
    if (not simpleShader_.IsReady())
        return;

    simpleShader_.Start();

    for (const auto& sub : debugObjects_)
    {
        if (not sub.model)
            continue;


        //const auto& rcomp = sub->GetComponent<RendererComponent>;
        //auto model        = rcomp->GetMohadedelWrapper().Get(LevelOfDetail::L1);

        //if (model == nullptr)
        //    continue;

        //RenderModel(sub, *model);
    }

    simpleShader_.Stop();
}
