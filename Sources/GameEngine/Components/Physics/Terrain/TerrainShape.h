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
    TerrainShape(ComponentContext&, GameObject&);
    void setScale(const vec3&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    TerrainShape& SetHeightMap(const File&);
    HeightMap* GetHeightMap();
    const File& GetHeightMapFile() const;
    std::optional<float> GetHeightOfTerrain(const vec3&) const;

private:
    void OnAwake();
    void LoadHeightMap(const File&);
    void create();

private:
    std::unique_ptr<TerrainHeightGetter> terrainHeightGetter_;
    Components::TerrainRendererComponent* terrainRendererComponent_;
    vec2ui size_;
    HeightMap* heightMap_;
    File heightMapFile_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static const std::string name;
};
}  // namespace Components
}  // namespace GameEngine
