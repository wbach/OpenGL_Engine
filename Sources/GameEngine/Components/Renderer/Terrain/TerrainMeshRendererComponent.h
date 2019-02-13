#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "TerrainTexturesTypes.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace GameEngine
{
namespace Components
{
class TerrainMeshRendererComponent : public BaseComponent
{
public:
    TerrainMeshRendererComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~TerrainMeshRendererComponent() override;

    TerrainMeshRendererComponent& LoadTextures(const std::unordered_map<TerrainTextureType, std::string>&);

    ModelWrapper& GetModel();
    Texture* GetTexture(TerrainTextureType type);
    const TerrainTexturesMap& GetTextures() const;
    const std::unordered_map<TerrainTextureType, std::string>& GetTextureFileNames() const;
    inline const std::vector<BufferObject>& GetPerObjectUpdateBuffers() const;

private:
    virtual void ReqisterFunctions() override;
    void SetTexture(TerrainTextureType, Texture*);
    void LoadHeightMap(const std::string& terrainFile);
    void Subscribe();
    void UnSubscribe();

private:
    ModelWrapper modelWrapper_;
    TerrainTexturesMap textures_;
    std::unordered_map<TerrainTextureType, std::string> texturedFileNames_;

    std::vector<PerObjectUpdate> perObjectUpdate_;  // for each mesh
    std::vector<BufferObject> perObjectUpdateBuffer_;

public:
    static ComponentsType type;
};

const std::vector<BufferObject>& TerrainMeshRendererComponent::GetPerObjectUpdateBuffers() const
{
    return perObjectUpdateBuffer_;
}

}  // namespace Components
}  // namespace GameEngine
