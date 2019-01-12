#pragma once
#include <vector>
#include "GameEngine/Components/Physics/CollisionShape.h"
#include "TerrainHeightGetter.h"

namespace GameEngine
{
namespace Components
{
class TerrainShape : public CollisionShape
{
public:
    TerrainShape(const ComponentContext& componentContext, GameObject& gameObject);
    void Update();
    virtual void ReqisterFunctions() override;
    TerrainShape& SetHeightMap(const std::string& filename);
    TerrainShape& SetHeightFactor(float factor);
    TerrainShape& SetData(std::vector<float>* data);

    std::optional<float> GetHeightofTerrain(float worldX, float worldZ);

private:
    void OnAwake();
    void LoadHeightMap(const std::string& hightMapFile);

private:
    vec2ui size_;
    float heightFactor_;
    std::vector<float>* data_;
    std::string heightMapFilename_;
    std::unique_ptr<TerrainHeightGetter> terrainHeightGetter_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
