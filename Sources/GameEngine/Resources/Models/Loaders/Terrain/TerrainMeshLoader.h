#pragma once
#include "GameEngine/Resources/Models/AbstractLoader.h"
#include "Types.h"

namespace GameEngine
{
class ITextureLoader;
class TerrainConfiguration;
class TerrainHeightTools;
class HeightMap;

class TerrainMeshLoader : public AbstractLoader
{
public:
    TerrainMeshLoader(ITextureLoader& textureLodaer);
    bool ParseFile(const File&) override;
    bool CheckExtension(const File&) override;
    std::unique_ptr<Model> Create() override;
    std::unique_ptr<Model> createModel(const HeightMap&, std::optional<uint32>);

private:
    void CreateAsSingleTerrain(Model&, TerrainHeightTools&, uint32);
    void CreatePartial(Model&, TerrainHeightTools&, uint32 partsCount, uint32);
    void ReserveMeshData(GameEngine::Mesh& mesh, uint32 size);
    void CreateTerrainVertexes(TerrainHeightTools&, GameEngine::Mesh&, uint32 x_start, uint32 y_start, uint32 width,
                               uint32 height, uint32 resolutionDivideFactor);
    void CreateIndicies(GameEngine::Mesh& mesh, IndicesDataType size);

private:
    std::unique_ptr<Model> model_;
};
}  // namespace GameEngine
