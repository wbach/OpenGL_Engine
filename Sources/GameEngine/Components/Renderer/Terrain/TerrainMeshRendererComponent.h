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
    inline const GraphicsApi::ID& GetPerObjectUpdateBuffer(uint32 id) const;

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

    std::vector<BufferObject<PerObjectUpdate>> perObjectUpdateBuffer_;

public:
    static ComponentsType type;
};

inline const GraphicsApi::ID& TerrainMeshRendererComponent::GetPerObjectUpdateBuffer(uint32 id) const
{
    return perObjectUpdateBuffer_[id].GetId();
}

}  // namespace Components
}  // namespace GameEngine
