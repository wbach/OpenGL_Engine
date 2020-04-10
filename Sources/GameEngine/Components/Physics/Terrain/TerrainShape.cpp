
#include "TerrainShape.h"

#include <algorithm>
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/TextureLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
ComponentsType TerrainShape::type = ComponentsType::TerrainShape;

TerrainShape::TerrainShape(const ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(ComponentsType::TerrainShape, componentContext, gameObject)
    , terrainHeightGetter_(nullptr)
    , terrainRendererComponent_(nullptr)
    , size_(1.f)
    , heightMap_(nullptr)
{
}
void TerrainShape::Update()
{
}
void TerrainShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&TerrainShape::OnAwake, this));
}
void TerrainShape::OnAwake()
{
    terrainRendererComponent_ = thisObject_.GetComponent<TerrainRendererComponent>();

    if (terrainRendererComponent_)
    {
        LoadHeightMap(heightMapFile_);
        const auto& scale = terrainRendererComponent_->GetScale();
        const auto& data  = heightMap_->GetImage().floatData;
        collisionShapeId_ = componentContext_.physicsApi_.CreateTerrainColider(positionOffset_, size_, data, scale);
    }
    else
    {
        ERROR_LOG("TerrainRendererComponent not found !");
    }
}
TerrainShape& TerrainShape::SetHeightMap(const std::string& filename)
{
    heightMapFile_ = filename;
    return *this;
}
void TerrainShape::LoadHeightMap(const std::string& hightMapFile)
{
    heightMapFile_        = hightMapFile;
    auto heightMapTexture = componentContext_.resourceManager_.GetTextureLaoder().LoadHeightMap(
        EngineConf_GetFullDataPathAddToRequierd(hightMapFile), true);

    if (not heightMapTexture)
    {
        return;
    }

    heightMap_ = static_cast<HeightMap*>(heightMapTexture);

    size_.x = heightMap_->GetImage().width;
    size_.y = heightMap_->GetImage().height;

    if (terrainRendererComponent_)
    {
        vec2 terrainPosition(0.f);  // To do position terrain in grid of terrains
        terrainHeightGetter_ = std::make_unique<TerrainHeightGetter>(
            terrainRendererComponent_->GetTerrainConfiguration(), *heightMap_, terrainPosition);
    }
    else
    {
        ERROR_LOG("terrainHeightGetter creating error! terrainRendererComponent not found.");
    }
}
const std::string TerrainShape::GetHeightMapFileName() const
{
    return heightMapFile_;
}

std::optional<float> TerrainShape::GetHeightOfTerrain(const vec3& worldPosition) const
{
    if (not terrainHeightGetter_)
        return std::nullopt;

    return terrainHeightGetter_->GetHeightofTerrain(worldPosition.x, worldPosition.z);
}

HeightMap* TerrainShape::GetHeightMap()
{
    return heightMap_;
}
}  // namespace Components
}  // namespace GameEngine
