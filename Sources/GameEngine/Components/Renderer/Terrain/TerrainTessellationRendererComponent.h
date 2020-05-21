#pragma once
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "TerrainComponentBase.h"
#include "TerrainConfiguration.h"
#include "TerrainQuadTree.h"
#include "TerrainTexturesTypes.h"

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
    void HeightMapChanged();

private:
    void LoadHeightMap(const File&) override;
    void UpdateHeightMap(const File&) override;
    void Update();

private:
    TerrainQuadTree terrainQuadTree_;
    float normalStrength_;
};

}  // namespace Components
}  // namespace GameEngine
