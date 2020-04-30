#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "TerrainTexturesTypes.h"
#include "TerrainConfiguration.h"
#include <GraphicsApi/IGraphicsApi.h>

namespace GameEngine
{
class HeightMap;

namespace Components
{
class TerrainMeshRendererComponent : public BaseComponent
{
public:
    TerrainMeshRendererComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~TerrainMeshRendererComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;

    TerrainMeshRendererComponent& LoadTextures(const std::unordered_map<TerrainTextureType, std::string>&);

    ModelWrapper& GetModel();
    Texture* GetTexture(TerrainTextureType type);
    const TerrainTexturesMap& GetTextures() const;
    const std::unordered_map<TerrainTextureType, std::string>& GetTextureFileNames() const;
    inline const GraphicsApi::ID& GetPerObjectUpdateBuffer(uint32 id) const;
    const TerrainConfiguration& GetConfiguration() const;
    void UpdateTexture(TerrainTextureType, const std::string&);
    HeightMap* GetHeightMap();
    void HeightMapChanged();

private:
    void UpdateMeshHeights();
    void UpdatePartialTerrainMeshes();
    void UpdateSingleTerrainMesh();
    void SetTexture(TerrainTextureType, Texture*);
    void UpdateTexture(TerrainTextureType, Texture*);
    void LoadHeightMap(const std::string& terrainFile);
    void Subscribe();
    void UnSubscribe();
    void CreateShaderBuffers(const GameEngine::Model&);
    BufferObject<PerObjectUpdate>& CreatePerObjectBuffer(GraphicsApi::IGraphicsApi&);
    void LoadObjectToGpu(GpuObject&);
    
private:
    void ReleaseModels();
    void ClearShaderBuffers();
    void ReleaseTextures();

private:
    TerrainConfiguration config_;
    ModelWrapper modelWrapper_;
    TerrainTexturesMap textures_;
    HeightMap* heightMap_;
    std::unordered_map<TerrainTextureType, std::string> texturedFileNames_;
    std::vector<std::unique_ptr<BufferObject<PerObjectUpdate>>> perObjectUpdateBuffer_;
    bool isSubscribed_;

public:
    static ComponentsType type;
};

inline const GraphicsApi::ID& TerrainMeshRendererComponent::GetPerObjectUpdateBuffer(uint32 id) const
{
    return perObjectUpdateBuffer_[id]->GetGraphicsObjectId();
}

}  // namespace Components
}  // namespace GameEngine
