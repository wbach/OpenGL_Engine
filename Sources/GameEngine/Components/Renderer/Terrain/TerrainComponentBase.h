#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "PerTerrainTexturesBuffer.h"
#include "TerrainConfiguration.h"
#include "TerrainTexturesTypes.h"
#include "GameEngine/Resources/TextureParameters.h"
#include <vector>
#include <utility>
#include <functional>

namespace GameEngine
{
class HeightMap;

namespace Components
{
class TerrainComponentBase
{
public:
    struct TerrainTexture
    {
        File file;
        float tiledScale{1.f};
        TerrainTextureType type;
    };

    TerrainComponentBase(ComponentContext&, GameObject&);
    virtual ~TerrainComponentBase() = default;

    virtual std::vector<std::pair<FunctionType, std::function<void()>>> FunctionsToRegister() = 0;

    virtual void RecalculateNormals() = 0;
    virtual void HeightMapChanged() = 0;
    virtual void CleanUp();

    void BlendMapChanged();

    void LoadTextures(const std::vector<TerrainTexture>&);
    const File* getTextureFile(TerrainTextureType) const;
    TerrainTexture* getTerrainTexture(TerrainTextureType);
    void updateTerrainTextureBuffer();
    const std::vector<TerrainTexture>& GetInputDataTextures() const;
    const std::vector<std::pair<TerrainTextureType, Texture*>>& GetTextures() const;
    Texture* GetTexture(TerrainTextureType) const;
    const TerrainConfiguration& GetConfiguration() const;
    HeightMap* GetHeightMap();
    void UpdateTexture(TerrainTextureType, const File&);
    GraphicsApi::ID getPerTerrainTexturesBufferId() const;

protected:
    virtual void LoadHeightMap(const File&)   = 0;
    virtual void UpdateHeightMap(const File&) = 0;
    void LoadTerrainConfiguration(const File&);
    void SetTexture(TerrainTextureType, Texture*);
    void UpdateTexture(TerrainTextureType, Texture*);
    void Subscribe();
    void UnSubscribe();

private:
    void ReleaseTextures();

protected:
    ComponentContext& componentContext_;
    GameObject& thisObject_;
    std::unique_ptr<BufferObject<PerTerrainTexturesBuffer>> perTerrainTexturesBuffer_;

    TextureParameters heightMapParameters_;
    TerrainConfiguration config_;
    HeightMap* heightMap_;
    bool isSubscribed_;

    std::vector<TerrainTexture> inputData_;
    std::vector<std::pair<TerrainTextureType, Texture*>> textures_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
