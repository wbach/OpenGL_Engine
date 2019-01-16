#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "TerrainTexturesTypes.h"

namespace GameEngine
{
namespace Components
{
class TerrainMeshRendererComponent : public BaseComponent
{
public:
    TerrainMeshRendererComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~TerrainMeshRendererComponent() override;
    virtual void ReqisterFunctions() override;
    TerrainMeshRendererComponent& LoadTextures(const std::unordered_map<TerrainTextureType, std::string>&);
    const TerrainTexturesMap& GetTextures() const;
    Texture* GetTexture(TerrainTextureType type);
    ModelWrapper& GetModel();

private:
    void SetTexture(TerrainTextureType, Texture*);
    void LoadHeightMap(const std::string& terrainFile);
    void Subscribe();
    void UnSubscribe();

private:
    ModelWrapper modelWrapper_;
    TerrainTexturesMap textures_;
    std::unordered_map<TerrainTextureType, std::string> texturedFileNames_;

public:
    static ComponentsType type;
};

}  // namespace Components
}  // namespace GameEngine
