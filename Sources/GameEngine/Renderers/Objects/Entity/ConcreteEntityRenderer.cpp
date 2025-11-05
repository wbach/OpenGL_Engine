#include "ConcreteEntityRenderer.h"

#include <Logger/Log.h>
#include <Utils/MeasurementHandler.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GraphicsApi/ShaderProgramType.h"

namespace GameEngine
{
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
    LOG_DEBUG << "";
    shader_.Clear();
    instancesShader_.Clear();
}

void ConcreteEntityRenderer::init()
{
    LOG_DEBUG << "";
    shader_.Init();
    instancesShader_.Init();
    entityRenderer_.init();
}

void ConcreteEntityRenderer::render()
{
    if (shader_.IsReady())
    {
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
    LOG_DEBUG << "";
    shader_.Reload();
    instancesShader_.Reload();
}
}  // namespace GameEngine
