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

    void RecalculateYOffset() override;
    void RecalculateNormals() override;
    void CleanUp() override;
    const TerrainQuadTree& GetTree() const;
    Texture* GetNormalMap() const;
    void SetNormalMap(std::unique_ptr<Texture>);
    void HeightMapChanged();

private:
    void LoadHeightMap(const std::string&) override;
    void UpdateHeightMap(const std::string&) override;
    void Update();

private:
    TerrainQuadTree terrainQuadTree_;
    std::unique_ptr<Texture> normalMap_;
    float normalStrength_;
};

}  // namespace Components
}  // namespace GameEngine
