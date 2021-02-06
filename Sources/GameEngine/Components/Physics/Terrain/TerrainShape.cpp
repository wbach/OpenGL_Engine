
#include "TerrainShape.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/TextureLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string CSTR_HEIGHTMAP_FILENAME = "heightMapFileName";
}  // namespace

const std::string TerrainShape::name = {"TerrainShape"};

TerrainShape::TerrainShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(typeid(TerrainShape).hash_code(), componentContext, gameObject)
    , terrainHeightGetter_(nullptr)
    , terrainRendererComponent_(nullptr)
    , size_(1)
    , heightMap_(nullptr)
{
}

void TerrainShape::CleanUp()
{
    if (heightMap_)
        componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*heightMap_);

    CollisionShape::CleanUp();
}
void TerrainShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&TerrainShape::OnAwake, this));
}
void TerrainShape::OnAwake()
{
    LoadHeightMap(heightMapFile_);
    create();
}
TerrainShape& TerrainShape::SetHeightMap(const File& filename)
{
    heightMapFile_ = filename;
    return *this;
}
void TerrainShape::LoadHeightMap(const File& hightMapFile)
{
    heightMapFile_ = hightMapFile;

    TextureParameters params;
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;

    auto heightMapTexture = componentContext_.resourceManager_.GetTextureLoader().LoadHeightMap(hightMapFile, params);

    if (not heightMapTexture)
    {
        return;
    }

    heightMap_ = static_cast<HeightMap*>(heightMapTexture);

    size_.x = heightMap_->GetImage().width;
    size_.y = heightMap_->GetImage().height;

    if (terrainRendererComponent_)
    {
        terrainHeightGetter_ =
            std::make_unique<TerrainHeightGetter>(thisObject_.GetWorldTransform().GetScale(), *heightMap_,
                                                  thisObject_.GetTransform().GetPosition());
    }
    else
    {
        ERROR_LOG("terrainHeightGetter creating error! terrainRendererComponent not found.");
    }
}
void TerrainShape::create()
{
    collisionShapeId_ = componentContext_.physicsApi_.CreateTerrainColider(positionOffset_, *heightMap_, thisObject_.GetWorldTransform().GetScale());
}
void TerrainShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<TerrainShape>(componentContext, gameObject);

        vec3 positionOffset(0.f);
        ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
        component->SetPostionOffset(positionOffset);

        std::string heightmap;
        ::Read(node.getChild(CSTR_HEIGHTMAP_FILENAME), heightmap);

        if (not heightmap.empty())
        {
            component->SetHeightMap(heightmap);
        }

        return component;
    };
    regsiterComponentReadFunction(TerrainShape::name, readFunc);
}
void TerrainShape::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, TerrainShape::name});

    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_HEIGHTMAP_FILENAME), GetHeightMapFile().GetDataRelativeDir());
}
const File& TerrainShape::GetHeightMapFile() const
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
