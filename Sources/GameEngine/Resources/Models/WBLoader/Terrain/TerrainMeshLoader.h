#pragma once
#include "../AbstractLoader.h"
#include "Types.h"

namespace GameEngine
{
class ITextureLoader;
class TerrainConfiguration;
class TerrainHeightTools;
class HeightMap;

namespace WBLoader
{
class TerrainMeshLoader : public AbstractLoader
{
public:
    TerrainMeshLoader(ITextureLoader& textureLodaer);
    virtual void ParseFile(const File&) override;
    virtual bool CheckExtension(const std::string& filename) override;
    virtual std::unique_ptr<Model> Create() override;
    std::unique_ptr<Model> createModel(const HeightMap&);

private:
    void CreateAsSingleTerrain(Model&, TerrainHeightTools&);
    void CreatePartial(Model&, TerrainHeightTools&, uint32 partsCount);
    void ReserveMeshData(GameEngine::Mesh& mesh, uint32 size);
    void CreateTerrainVertexes(TerrainHeightTools&, GameEngine::Mesh& mesh, uint32 x_start, uint32 y_start,
                               uint32 width, uint32 height);
    void CreateIndicies(GameEngine::Mesh& mesh, IndicesDataType size);

private:
    std::unique_ptr<Model> model_;
};
}  // namespace WBLoader
}  // namespace GameEngine
