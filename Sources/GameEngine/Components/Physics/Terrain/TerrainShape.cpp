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
    : CollisionShape(GetComponentType<TerrainShape>(), componentContext, gameObject)
    , terrainHeightGetter_(nullptr)
    , terrainRendererComponent_(nullptr)
    , size_(1)
    , heightMap_(nullptr)
{
}
void TerrainShape::setScale(const vec3& scale)
{
    if (heightMap_)
    {
        float scaleX = scale.x / static_cast<float>(heightMap_->GetImage().width - 1);
        float scaleY = scale.z / static_cast<float>(heightMap_->GetImage().height - 1);

        CollisionShape::setScale(vec3(scaleX, scale.y, scaleY));

        auto rigidBody = thisObject_.GetComponent<Rigidbody>();
        if (rigidBody)
        {
            auto offset = heightMap_->GetMaximumHeight() * scale.y - (heightMap_->GetDeltaHeight() * scale.y / 2.f);
            rigidBody->SetPosition(positionOffset + vec3(0, offset, 0));
        }
    }
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
    LoadHeightMapIfSet();
    create();
}
TerrainShape& TerrainShape::SetHeightMap(const File& filename)
{
    heightMapFile = filename;
    return *this;
}
void TerrainShape::LoadHeightMapIfSet()
{
    if (heightMapFile.empty())
    {
        return;
    }

    TextureParameters params;
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;

    auto heightMapTexture = componentContext_.resourceManager_.GetTextureLoader().LoadHeightMap(heightMapFile, params);

    if (not heightMapTexture)
    {
        return;
    }

    heightMap_ = static_cast<HeightMap*>(heightMapTexture);

    size_.x = heightMap_->GetImage().width;
    size_.y = heightMap_->GetImage().height;

    terrainRendererComponent_ = thisObject_.GetComponent<TerrainRendererComponent>();
    if (terrainRendererComponent_)
    {
        terrainHeightGetter_ = std::make_unique<TerrainHeightGetter>(thisObject_.GetWorldTransform().GetScale() * heightMap_->GetScale(), *heightMap_,
                                                                     thisObject_.GetLocalTransform().GetPosition());
    }
    else
    {
        LOG_ERROR << "terrainHeightGetter creating error! terrainRendererComponent not found.";
    }
}
void TerrainShape::create()
{
    if (not heightMap_)
    {
        terrainRendererComponent_ = thisObject_.GetComponent<TerrainRendererComponent>();
        if (terrainRendererComponent_)
        {
            heightMap_ = terrainRendererComponent_->GetHeightMap();
        }
    }

    if (heightMap_)
    {
        collisionShapeId_ = componentContext_.physicsApi_.CreateTerrainColider(
            positionOffset, thisObject_.GetWorldTransform().GetScale() * heightMap_->GetScale(), *heightMap_);
    }
    else
    {
        LOG_ERROR << "Collision shape create error, heightMap not set ";
    }
}
void TerrainShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
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
    regsiterComponentReadFunction(GetComponentType<TerrainShape>(), readFunc);
}
void TerrainShape::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, TerrainShape::name});

    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    if (not heightMapFile.empty())
    {
        ::write(node.addChild(CSTR_HEIGHTMAP_FILENAME), GetHeightMapFile().GetDataRelativePath());
    }
}
const File& TerrainShape::GetHeightMapFile() const
{
    return heightMapFile;
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
