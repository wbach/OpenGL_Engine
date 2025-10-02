#pragma once
#include "GameEngine/Components/Physics/CollisionShape.h"

#include "GameEngine/Resources/File.h"

namespace GameEngine
{
class HeightMap;
class TerrainHeightGetter;

namespace Components
{
class TerrainRendererComponent;
}

namespace Components
{
class TerrainShape : public CollisionShape
{
public:
    File heightMapFile;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(heightMapFile)
        FIELD_VECTOR3F(positionOffset)
    END_FIELDS()
    // clang-format on

public:
    TerrainShape(ComponentContext&, GameObject&);
    void setScale(const vec3&) override;
    void CleanUp() override;
    void ReqisterFunctions() override;

    TerrainShape& SetHeightMap(const File&);
    HeightMap* GetHeightMap();
    const File& GetHeightMapFile() const;
    std::optional<float> GetHeightOfTerrain(const vec3&) const;

private:
    void InitShape() override;
    void LoadHeightMapIfSet();
    void create();

private:
    std::unique_ptr<TerrainHeightGetter> terrainHeightGetter_;
    Components::TerrainRendererComponent* terrainRendererComponent_;
    vec2ui size_;
    HeightMap* heightMap_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static const std::string name;
};
}  // namespace Components
}  // namespace GameEngine
