#pragma once
#include "TerrainComponentBase.h"
#include "TerrainQuadTree.h"

namespace GameEngine
{
class HeightMap;

namespace Components
{
class TerrainTessellationRendererComponent : public TerrainComponentBase
{
public:
    TerrainTessellationRendererComponent(ComponentContext&, GameObject&);
    ~TerrainTessellationRendererComponent() override;

    std::vector<std::pair<FunctionType, std::function<void()>>> FunctionsToRegister() override;

    void RecalculateNormals() override;
    void CleanUp() override;
    const TerrainQuadTree& GetTree() const;
    void HeightMapChanged() override;

private:
    void LoadHeightMap(const File&) override;
    void UpdateHeightMap(const File&) override;
    void Update();

private:
    TerrainQuadTree terrainQuadTree_;
};

}  // namespace Components
}  // namespace GameEngine
