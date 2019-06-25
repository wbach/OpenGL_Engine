
#include "TerrainShape.h"

#include <algorithm>
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainDef.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
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
    , size_(1)
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
    if (heightMap_ == nullptr)
    {
        ERROR_LOG("Height data is nullptr, SetHeightMap need to be called.");
        return;
    }

    vec3 scale(6000.f, 800.f, 6000.f);

    auto& data           = heightMap_->GetImage()->floatData;
    auto renderComponent = thisObject_.GetComponent<TerrainRendererComponent>();

    if (renderComponent)
    {
        const auto& scaleXY = renderComponent->GetConfig().GetScaleXZ();
        scale.x             = scaleXY;
        scale.y             = renderComponent->GetConfig().GetScaleY();
        scale.z             = scaleXY;
    }

    collisionShapeId_ = componentContext_.physicsApi_.CreateTerrainColider(positionOffset_, size_, data, scale);
}
TerrainShape& TerrainShape::SetHeightMap(const std::string& filename)
{
    heightMapFile_ = filename;
    LoadHeightMap(filename);
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

    size_.x = heightMap_->GetImage()->width;
    size_.y = heightMap_->GetImage()->height;
}
const std::string TerrainShape::GetHeightMapFileName() const
{
    return heightMapFile_;
}
HeightMap* TerrainShape::GetHeightMap()
{
    return heightMap_;
}
}  // namespace Components
}  // namespace GameEngine
