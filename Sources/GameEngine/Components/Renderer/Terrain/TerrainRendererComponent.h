#pragma once
#include <Types.h>

#include <unordered_map>

#include "GameEngine/Components/BaseComponent.h"
#include "TerrainComponentBase.h"
#include "TerrainConfiguration.h"
#include "TerrainTexturesTypes.h"

namespace GameEngine
{
class HeightMap;

namespace Components
{
class TerrainMeshRendererComponent;
class TerrainTessellationRendererComponent;

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
    void InitFromParams(const std::unordered_map<std::string, std::string>&) override;
    std::unordered_map<ParamName, Param> GetParams() const override;

    void RecalculateNormals();
    void HeightMapChanged();
    void BlendMapChanged();
    void UpdateTexture(TerrainTextureType, const std::string&);
    TerrainRendererComponent& LoadTextures(const std::vector<TerrainComponentBase::TerrainTexture>&);
    Texture* GetTexture(TerrainTextureType) const;

    HeightMap* createHeightMap(const vec2ui&);
    HeightMap* GetHeightMap();
    const TerrainConfiguration& GetTerrainConfiguration() const;
    const std::vector<TerrainComponentBase::TerrainTexture>& GetInputDataTextures() const;

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
