#include "ConcreteTerrainMeshRenderer.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
namespace
{
const uint32 CASCADE_INDEX0 = 0;
const uint32 CASCADE_INDEX1 = 1;
const uint32 CASCADE_INDEX2 = 2;
const uint32 CASCADE_INDEX3 = 3;
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
    //context_.graphicsApi_.EnableCulling();
    //context_.graphicsApi_.EnableBlend();
    shader_.Start();
    bindShadowMap(CASCADE_INDEX0, 0);
    bindShadowMap(CASCADE_INDEX1, 22);
    bindShadowMap(CASCADE_INDEX2, 23);
    bindShadowMap(CASCADE_INDEX3, 24);
    *measurementValue_ = std::to_string(terrainMeshRenderer_.renderSubscribers());
    shader_.Stop();
}
void ConcreteTerrainMeshRenderer::bindShadowMap(uint32 id, uint32 nr) const
{
    if (context_.cascadedShadowMapsIds_[id])
    {
        context_.graphicsApi_.ActiveTexture(nr);
        context_.graphicsApi_.BindTexture(*context_.cascadedShadowMapsIds_[id]);
    }
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
