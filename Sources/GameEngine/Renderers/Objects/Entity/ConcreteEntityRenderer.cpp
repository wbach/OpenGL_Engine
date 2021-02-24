#include "ConcreteEntityRenderer.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GraphicsApi/ShaderProgramType.h"

namespace GameEngine
{
const uint32 CASCADE_INDEX0 = 0;
const uint32 CASCADE_INDEX1 = 1;
const uint32 CASCADE_INDEX2 = 2;
const uint32 CASCADE_INDEX3 = 3;

ConcreteEntityRenderer::ConcreteEntityRenderer(RendererContext& context)
    : context_(context)
    , entityRenderer_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Entity)
    , instancesShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::InstancesEntity)
{
    measurementValue_ = &context.measurmentHandler_.AddNewMeasurment("EnityRendererdMeshes", "0");
}

ConcreteEntityRenderer::~ConcreteEntityRenderer()
{
    DEBUG_LOG("");
    unSubscribeAll();
}

void ConcreteEntityRenderer::init()
{
    DEBUG_LOG("");
    shader_.Init();
    instancesShader_.Init();
    entityRenderer_.init();
}

void ConcreteEntityRenderer::render()
{
    if (shader_.IsReady())
    {
        bindShadowMap(CASCADE_INDEX0, 4); // enity bind material texture + 1
        bindShadowMap(CASCADE_INDEX1, 5);
        bindShadowMap(CASCADE_INDEX2, 6);
        bindShadowMap(CASCADE_INDEX3, 7);

        if (EngineConf.renderer.useInstanceRendering)
        {
            *measurementValue_ = std::to_string(entityRenderer_.renderEntityWithGrouping(shader_, instancesShader_));
        }
        else
        {
            shader_.Start();
            *measurementValue_ = std::to_string(entityRenderer_.renderEntitiesWithoutGrouping());
            shader_.Stop();
        }
    }
}

void ConcreteEntityRenderer::subscribe(GameObject& gameObject)
{
    entityRenderer_.subscribe(gameObject);
}

void ConcreteEntityRenderer::unSubscribe(GameObject& gameObject)
{
    entityRenderer_.unSubscribe(gameObject);
}

void ConcreteEntityRenderer::unSubscribeAll()
{
    entityRenderer_.unSubscribeAll();
}

void ConcreteEntityRenderer::reloadShaders()
{
    DEBUG_LOG("");
    shader_.Reload();
}

void ConcreteEntityRenderer::bindShadowMap(uint32 id, uint32 nr) const
{
    if (context_.cascadedShadowMapsIds_[id])
    {
        context_.graphicsApi_.ActiveTexture(nr);
        context_.graphicsApi_.BindTexture(*context_.cascadedShadowMapsIds_[id]);
    }
}

}  // namespace GameEngine
