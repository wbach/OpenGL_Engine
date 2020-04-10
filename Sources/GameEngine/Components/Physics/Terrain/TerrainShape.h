#pragma once
#include <vector>
#include "GameEngine/Components/Physics/CollisionShape.h"
#include "TerrainHeightGetter.h"

namespace GameEngine
{
class HeightMap;

namespace Components
{
class TerrainRendererComponent;
}

namespace Components
{
class TerrainShape : public CollisionShape
{
public:
    TerrainShape(const ComponentContext& componentContext, GameObject& gameObject);
    void Update();
    virtual void ReqisterFunctions() override;
    TerrainShape& SetHeightMap(const std::string& filename);
    HeightMap* GetHeightMap();
    const std::string GetHeightMapFileName() const;
    std::optional<float> GetHeightOfTerrain(const vec3& worldPosition) const;

private:
    void OnAwake();
    void LoadHeightMap(const std::string& hightMapFile);

private:
    std::unique_ptr<TerrainHeightGetter> terrainHeightGetter_;
    Components::TerrainRendererComponent* terrainRendererComponent_;
    vec2ui size_;
    HeightMap* heightMap_;
    std::string heightMapFile_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
