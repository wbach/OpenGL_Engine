#include "ConcreteTerrainMeshRenderer.h"

#include <Logger/Log.h>

#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RendererContext.h"
namespace GameEngine
{
namespace
{
const std::string TERRAIN_MEASURMENT_NAME{"TerrainMeshesRendered"};
}  // namespace
ConcreteTerrainMeshRenderer::ConcreteTerrainMeshRenderer(RendererContext& context)
    : context_(context)
    , terrainMeshRenderer_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TerrainMesh)
{
    measurementValue_ = &context.measurmentHandler_.AddNewMeasurment(TERRAIN_MEASURMENT_NAME, "0");
}
ConcreteTerrainMeshRenderer::~ConcreteTerrainMeshRenderer()
{
}
void ConcreteTerrainMeshRenderer::init()
{
    shader_.Init();
}
void ConcreteTerrainMeshRenderer::render()
{
    shader_.Start();
    *measurementValue_ = std::to_string(terrainMeshRenderer_.renderSubscribers());
    shader_.Stop();
}
void ConcreteTerrainMeshRenderer::subscribe(GameObject& gameObject)
{
    terrainMeshRenderer_.subscribe(gameObject);
}
void ConcreteTerrainMeshRenderer::unSubscribe(GameObject& gameObject)
{
    terrainMeshRenderer_.unSubscribe(gameObject);
}
void ConcreteTerrainMeshRenderer::reloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
