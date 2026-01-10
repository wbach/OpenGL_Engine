#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <functional>
#include <optional>
#include <utility>
#include <vector>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/ShaderBufferObject.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "PerTerrainTexturesBuffer.h"
#include "TerrainConfiguration.h"
#include "TerrainTexture.h"
#include "TerrainTexturesTypes.h"

namespace GameEngine
{
class HeightMap;

namespace Components
{
class TerrainComponentBase
{
public:
    TerrainComponentBase(ComponentContext&, GameObject&, std::vector<TerrainTexture>&);
    virtual ~TerrainComponentBase() = default;

    virtual std::vector<std::pair<FunctionType, std::function<void()>>> FunctionsToRegister() = 0;

    virtual void RecalculateNormals() = 0;
    virtual void HeightMapChanged()   = 0;
    virtual void CleanUp();
    virtual void Reload();

    void BlendMapChanged();

    void updateTerrainTextureBuffer();
    void setTexture(Texture&, float, TerrainTextureType);
    void LoadTexture(const TerrainTexture&);
    void LoadTextures(const std::vector<TerrainTexture>&);

    void ChangeTexture(const File&, const File&);
    void RemoveTexture(const File&);

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
    std::optional<File> ConvertObjectToHeightMap(const File&, uint32 heightmapResultuion = 2048,
                                                 const std::optional<File>& outputfile = std::nullopt) const;

protected:
    virtual void LoadHeightMap(const File&)   = 0;
    virtual void UpdateHeightMap(const File&) = 0;
    void LoadTextureImpl(const TerrainTexture&);
    void LoadTerrainConfiguration(const File&);
    void SetTexture(TerrainTextureType, Texture*);
    void UpdateTexture(TerrainTextureType, Texture*);
    void Subscribe();
    void UnSubscribe();
    void updateTerrainTextureBufferData();

private:
    void ReleaseTextures();

protected:
    ComponentContext& componentContext_;
    GameObject& thisObject_;
    std::unique_ptr<ShaderBufferObject<PerTerrainTexturesBuffer>> perTerrainTexturesBuffer_;

    TextureParameters heightMapParameters_;
    TerrainConfiguration config_;
    HeightMap* heightMap_;
    bool isSubscribed_;

    std::vector<TerrainTexture>& inputData_;
    std::vector<std::pair<TerrainTextureType, Texture*>> textures_;
};
}  // namespace Components
}  // namespace GameEngine
