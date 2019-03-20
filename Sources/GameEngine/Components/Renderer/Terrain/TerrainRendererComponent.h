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
class TerrainRendererComponent : public BaseComponent
{
public:
    TerrainRendererComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~TerrainRendererComponent();
    virtual void ReqisterFunctions() override;
    TerrainRendererComponent& LoadTextures(const std::unordered_map<TerrainTextureType, std::string>&);
    const TerrainTexturesMap& GetTextures() const;
    Texture* GetTexture(TerrainTextureType type);
    const std::unordered_map<TerrainTextureType, std::string>& GetTextureFileNames() const;
    const TerrainQuadTree& GetTree() const;
    const TerrainConfiguration& GetConfig() const;
    void SetNormalMap(std::unique_ptr<Texture> normalMap);
    std::optional<uint32> GetNormalMapId() const;
    Texture* GetHeightMap() const;

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
    Texture* normalMap_;
    Texture* heightMap_;

public:
    static ComponentsType type;
};

}  // namespace Components
}  // namespace GameEngine
