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
    virtual void ReqisterFunctions() override;
    TerrainRendererComponent& LoadTextures(const std::unordered_map<TerrainTextureType, std::string>&);
    const TerrainTexturesMap& GetTextures() const;
    Texture* GetTexture(TerrainTextureType type);
    const std::unordered_map<TerrainTextureType, std::string>& GetTextureFileNames() const;
    const TerrainQuadTree& GetTree() const;
    const TerrainConfiguration& GetConfig() const;

private:
    void SetTexture(TerrainTextureType, Texture*);
    void LoadHeightMap(const std::string& hightMapFile);
    void Subscribe();
    void UnSubscribe();

private:
    TerrainQuadTree terrainQuadTree_;
    TerrainConfiguration terrainConfiguration_;

    TerrainTexturesMap textures_;
    std::unordered_map<TerrainTextureType, std::string> texturedFileNames_;

public:
    static ComponentsType type;
};

}  // namespace Components
}  // namespace GameEngine
