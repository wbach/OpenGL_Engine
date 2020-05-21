#pragma once
#include "../AbstractLoader.h"
#include "Types.h"

namespace GameEngine
{
class ITextureLoader;
class TerrainConfiguration;
class TerrainHeightTools;

namespace WBLoader
{
class TerrainMeshLoader : public AbstractLoader
{
public:
    TerrainMeshLoader(ITextureLoader& textureLodaer);
    virtual void ParseFile(const File&) override;
    virtual bool CheckExtension(const std::string& filename) override;
    virtual std::unique_ptr<Model> Create() override;

private:
    void CreateAsSingleTerrain(TerrainHeightTools&);
    void CreatePartial(TerrainHeightTools&, uint32 partsCount);
    void ReserveMeshData(GameEngine::Mesh& mesh, uint32 size);
    void CreateTerrainVertexes(TerrainHeightTools&, GameEngine::Mesh& mesh, uint32 x_start, uint32 y_start, uint32 width, uint32 height);
    void CreateIndicies(GameEngine::Mesh& mesh, IndicesDataType size);
    void Clear();

private:
    std::unique_ptr<Model> model_;

    uint32 heightMapResolution_;
    vec3 terrainScale_;
};
}  // WBLoader
} // namespace GameEngine
