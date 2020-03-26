#pragma once
#include "../AbstractLoader.h"
#include "Types.h"

namespace GameEngine
{
class ITextureLoader;
class TerrainConfiguration;

namespace WBLoader
{
class TerrainMeshLoader : public AbstractLoader
{
public:
    TerrainMeshLoader(ITextureLoader& textureLodaer);
    virtual void ParseFile(const std::string& filename) override;
    virtual bool CheckExtension(const std::string& filename) override;
    virtual std::unique_ptr<Model> Create() override;

private:
    void ReserveMeshData(GameEngine::Mesh& mesh, uint32 size);
    void CreateTerrainVertexes(GameEngine::Mesh& mesh, uint32 x_start, uint32 y_start, uint32 width, uint32 height);
    vec3 CalculateNormalMap(uint32 x, uint32 z);
    float GetHeight(uint32 x, uint32 y) const;
    void CreateIndicies(GameEngine::Mesh& mesh, IndicesDataType size);
    void Clear();

private:
    std::unique_ptr<Model> model_;

    uint32 heightMapResolution_;
    FloatAttributeVec* heights_;
    float halfMaximumHeight_;
    vec3 terrainScale_;
};
}  // WBLoader
} // namespace GameEngine
