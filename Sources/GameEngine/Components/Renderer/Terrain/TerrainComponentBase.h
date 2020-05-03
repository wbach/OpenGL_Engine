#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
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
    TerrainComponentBase(ComponentContext&, GameObject&);
    virtual ~TerrainComponentBase() = default;

    virtual std::vector<std::pair<FunctionType, std::function<void()>>> FunctionsToRegister() = 0;

    virtual void RecalculateYOffset() = 0;
    virtual void RecalculateNormals() = 0;
    virtual void HeightMapChanged() = 0;
    virtual void CleanUp();

    void LoadTextures(const std::unordered_map<TerrainTextureType, std::string>&);
    const std::unordered_map<TerrainTextureType, std::string>& GetTextureFileNames() const;
    const TerrainTexturesMap& GetTextures() const;
    Texture* GetTexture(TerrainTextureType) const;
    const TerrainConfiguration& GetConfiguration() const;
    HeightMap* GetHeightMap();
    void UpdateTexture(TerrainTextureType, const std::string&);

protected:
    virtual void LoadHeightMap(const std::string&)   = 0;
    virtual void UpdateHeightMap(const std::string&) = 0;
    void LoadTerrainConfiguration(const std::string&);
    void SetTexture(TerrainTextureType, Texture*);
    void UpdateTexture(TerrainTextureType, Texture*);
    void Subscribe();
    void UnSubscribe();

private:
    void ReleaseTextures();

protected:
    ComponentContext& componentContext_;
    GameObject& thisObject_;

    TextureParameters heightMapParameters_;
    TerrainConfiguration config_;
    TerrainTexturesMap textures_;
    HeightMap* heightMap_;

    std::unordered_map<TerrainTextureType, std::string> texturedFileNames_;
    bool isSubscribed_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
