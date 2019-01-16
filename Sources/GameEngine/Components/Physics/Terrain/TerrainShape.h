#pragma once
#include <vector>
#include "GameEngine/Components/Physics/CollisionShape.h"
#include "TerrainHeightGetter.h"

namespace GameEngine
{
class HeightMap;

namespace Components
{
class TerrainShape : public CollisionShape
{
public:
    TerrainShape(const ComponentContext& componentContext, GameObject& gameObject);
    void Update();
    virtual void ReqisterFunctions() override;
    TerrainShape& SetHeightMap(const std::string& filename);

private:
    void OnAwake();
    void LoadHeightMap(const std::string& hightMapFile);

private:
    vec2ui size_;
    HeightMap* heightMap_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
