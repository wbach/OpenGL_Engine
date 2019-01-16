#include "TerrainShape.h"

#include <algorithm>
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainDef.h"
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
        Error("Height data is nullptr, SetHeightMap need to be called.");
        return;
    }

    auto& data = heightMap_->GetImage()->floatData;

    //for (auto& heigth : data)
    //{
    //    heigth *= 16.f;
    //}

    collisionShapeId_ =
        componentContext_.physicsApi_.CreateTerrainColider(positionOffset_, size_, data, 1.f);

    //auto minElementIter = std::min_element(data.begin(), data.end());
    //auto maxElementIter = std::max_element(data.begin(), data.end());

    //auto minElement = minElementIter != data.end() ? *minElementIter : 0.f;
    //auto maxElement = maxElementIter != data.end() ? *maxElementIter : 0.f;

    //auto distance = fabs(maxElement - minElement);
    //auto h        = maxElement - (distance / 2.f);
    //thisObject_.worldTransform.IncrasePosition(vec3(0, h, 0));
}
TerrainShape& TerrainShape::SetHeightMap(const std::string& filename)
{
    LoadHeightMap(filename);
    return *this;
}
void TerrainShape::LoadHeightMap(const std::string& hightMapFile)
{
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
}  // namespace Components
}  // namespace GameEngine
