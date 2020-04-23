#pragma once
#include <Types.h>
#include <unordered_map>
#include "GameEngine/Components/BaseComponent.h"
#include "TerrainTexturesTypes.h"
#include "TerrainConfiguration.h"

namespace GameEngine
{
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
    TerrainRendererComponent(const ComponentContext&, GameObject&);
    ~TerrainRendererComponent();

    TerrainRendererComponent& LoadTextures(const std::unordered_map<TerrainTextureType, std::string>&);
    const std::unordered_map<TerrainTextureType, std::string>& GetTextureFileNames() const;

    const TerrainConfiguration& GetTerrainConfiguration() const;
    RendererType GetRendererType() const;
    TerrainTessellationRendererComponent* GetTesselationTerrain();
    TerrainMeshRendererComponent* GetMeshTerrain();
    const vec3& GetScale() const;

    virtual void InitFromParams(std::unordered_map<std::string, std::string>) override;
    virtual std::unordered_map<ParamName, Param> GetParams() const override;

private:
    void SetRendererType(RendererType);
    virtual void ReqisterFunctions() override;

private:
    RendererType rendererType_;
    std::unique_ptr<TerrainMeshRendererComponent> meshComponent_;
    std::unique_ptr<TerrainTessellationRendererComponent> tesselationComponent_;
    bool functionsRegistered_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
