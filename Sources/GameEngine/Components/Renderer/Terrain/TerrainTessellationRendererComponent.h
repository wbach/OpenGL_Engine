#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "TerrainTexturesTypes.h"
#include "TerrainConfiguration.h"
#include "TerrainQuadTree.h"

namespace GameEngine
{
namespace Components
{
class TerrainTessellationRendererComponent : public BaseComponent
{
public:
    TerrainTessellationRendererComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~TerrainTessellationRendererComponent();
    virtual void ReqisterFunctions() override;
    TerrainTessellationRendererComponent& LoadTextures(const std::unordered_map<TerrainTextureType, std::string>&);
    const TerrainTexturesMap& GetTextures() const;
    Texture* GetTexture(TerrainTextureType type);
    const std::unordered_map<TerrainTextureType, std::string>& GetTextureFileNames() const;
    const TerrainQuadTree& GetTree() const;
    const TerrainConfiguration& GetConfig() const;
    Texture* GetNormalMap() const;
    Texture* GetHeightMap() const;
    void SetTexture(std::unique_ptr<Texture>);
    const vec3& GetScale() const;

private:
    void Update();
    void SetTexture(TerrainTextureType, Texture*);
    void LoadHeightMap(const std::string& hightMapFile);
    void Subscribe();
    void UnSubscribe();

private:
    TerrainConfiguration terrainConfiguration_;
    TerrainQuadTree terrainQuadTree_;

    TerrainTexturesMap textures_;
    std::unordered_map<TerrainTextureType, std::string> texturedFileNames_;
    std::unique_ptr<Texture> normalMap_;
    Texture* heightMap_;
    float normalStrength_;

public:
    static ComponentsType type;
};

}  // namespace Components
}  // namespace GameEngine
