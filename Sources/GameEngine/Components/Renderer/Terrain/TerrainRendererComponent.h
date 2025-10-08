#pragma once
#include <Types.h>

#include "GameEngine/Components/BaseComponent.h"
#include "TerrainTexture.h"

namespace GameEngine
{
class HeightMap;
class TerrainConfiguration;

namespace Components
{
class TerrainMeshRendererComponent;
class TerrainTessellationRendererComponent;
class TerrainComponentBase;

class TerrainRendererComponent : public BaseComponent
{
public:
    enum class RendererType
    {
        Mesh,
        Tessellation
    };

public:
    TerrainRendererComponent(ComponentContext&, GameObject&);
    ~TerrainRendererComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void RecalculateNormals();
    void HeightMapChanged();

    void BlendMapChanged();
    void setTexture(Texture&, float, TerrainTextureType);
    TerrainRendererComponent& LoadTextures(const std::vector<TerrainTexture>&);
    void UpdateTexture(TerrainTextureType, const std::string&);
    Texture* GetTexture(TerrainTextureType) const;

    HeightMap* createHeightMap(const vec2ui& = vec2ui(512));
    HeightMap* GetHeightMap();
    const TerrainConfiguration& GetTerrainConfiguration() const;
    const std::vector<TerrainTexture>& GetInputDataTextures() const;

    RendererType GetRendererType() const;
    TerrainTessellationRendererComponent* GetTesselationTerrain();
    TerrainMeshRendererComponent* GetMeshTerrain();

private:
    void SetRendererType(RendererType);

private:
    RendererType rendererType_;
    std::unique_ptr<TerrainComponentBase> terrainComponent_;
    bool functionsRegistered_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
