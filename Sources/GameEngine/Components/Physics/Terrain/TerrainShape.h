#pragma once
#include <vector>

#include "GameEngine/Components/Physics/CollisionShape.h"
#include "TerrainHeightGetter.h"
#include "GameEngine/Resources/File.h"

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
    TerrainShape(ComponentContext& componentContext, GameObject& gameObject);
    void CleanUp() override;
    void ReqisterFunctions() override;

    void Update();
    TerrainShape& SetHeightMap(const File&);
    HeightMap* GetHeightMap();
    const File& GetHeightMapFile() const;
    std::optional<float> GetHeightOfTerrain(const vec3& worldPosition) const;

private:
    void OnAwake();
    void LoadHeightMap(const File&);

private:
    std::unique_ptr<TerrainHeightGetter> terrainHeightGetter_;
    Components::TerrainRendererComponent* terrainRendererComponent_;
    vec2ui size_;
    HeightMap* heightMap_;
    File heightMapFile_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
