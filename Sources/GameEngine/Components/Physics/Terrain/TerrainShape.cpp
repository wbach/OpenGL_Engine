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
    , heightFactor_(1)
    , data_(nullptr)
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
    if (data_ == nullptr)
    {
        Error("Height data is nullptr, SetHeightMap need to be called.");
        return;
    }

    collisionShapeId_ =
        componentContext_.physicsApi_.CreateTerrainColider(positionOffset_, size_, *data_, heightFactor_);

    auto minElementIter = std::min_element(data_->begin(), data_->end());
    auto maxElementIter = std::max_element(data_->begin(), data_->end());

    auto minElement = minElementIter != data_->end() ? *minElementIter : 0.f;
    auto maxElement = maxElementIter != data_->end() ? *maxElementIter : 0.f;

    auto distance = fabs(maxElement - minElement);
    auto h        = maxElement - (distance / 2.f);
    thisObject_.worldTransform.IncrasePosition(vec3(0, h, 0));
}
TerrainShape& TerrainShape::SetHeightMap(const std::string& filename)
{
    heightMapFilename_ = filename;
    LoadHeightMap(filename);
    terrainHeightGetter_ = std::make_unique<TerrainHeightGetter>(size_, data_, thisObject_.worldTransform.GetPositionXZ());
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

    auto hightmap = static_cast<HeightMap*>(heightMapTexture);
    data_         = &hightmap->GetImage()->floatData;

    for (auto& d : *data_)
    {
        d *= heightFactor_;
    }

    size_.x = hightmap->GetImage()->width;
    size_.y = hightmap->GetImage()->height;
}
TerrainShape& TerrainShape::SetHeightFactor(float factor)
{
    heightFactor_ = factor;
    return *this;
}
TerrainShape& TerrainShape::SetData(std::vector<float>* data)
{
    data_ = data;
    return *this;
}
std::optional<float> TerrainShape::GetHeightofTerrain(float worldX, float worldZ)
{
    return terrainHeightGetter_ ?  terrainHeightGetter_->GetHeightofTerrain(worldX, worldZ) : std::optional<float>();
}
}  // namespace Components
}  // namespace GameEngine
