#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <functional>
#include <optional>
#include <utility>
#include <vector>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "PerTerrainTexturesBuffer.h"
#include "TerrainConfiguration.h"
#include "TerrainTexturesTypes.h"
#include "TerrainTexture.h"

namespace GameEngine
{
class HeightMap;

namespace Components
{
class TerrainComponentBase
{
public:
    TerrainComponentBase(ComponentContext&, GameObject&);
    virtual ~TerrainComponentBase() = default;

    virtual std::vector<std::pair<FunctionType, std::function<void()>>> FunctionsToRegister() = 0;

    virtual void RecalculateNormals() = 0;
    virtual void HeightMapChanged()   = 0;
    virtual void CleanUp();

    void BlendMapChanged();

    void updateTerrainTextureBuffer();
    void setTexture(Texture&, float, TerrainTextureType);
    void LoadTextures(const std::vector<TerrainTexture>&);

    const File* getTextureFile(TerrainTextureType) const;
    TerrainTexture* getTerrainTexture(TerrainTextureType);
    const std::vector<TerrainTexture>& GetInputDataTextures() const;
    const std::vector<std::pair<TerrainTextureType, Texture*>>& GetTextures() const;
    Texture* GetTexture(TerrainTextureType) const;
    const TerrainConfiguration& GetConfiguration() const;
    virtual HeightMap* createHeightMap(const vec2ui&);
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
    void updateTerrainTextureBufferData();
    std::optional<File> ConvertObjectToHeightMap(const File&) const;

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
};
}  // namespace Components
}  // namespace GameEngine
