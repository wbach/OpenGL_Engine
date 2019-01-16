#pragma once
#include "../AbstractLoader.h"
#include "Types.h"

namespace GameEngine
{
class ITextureLoader;

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
    void CreateTerrainVertexes(uint32 x_start, uint32 y_start, uint32 width, uint32 height);
    vec3 CalculateNormalMap(uint32 x, uint32 z);
    float GetHeight(uint32 x, uint32 y) const;
    void CreateMesh();
    void Clear();
    float FindMaxY(const FloatAttributeVec& v) const;
    void TranslateY(FloatAttributeVec&v, float y);

private:
    uint32 heightMapResolution_;
    IndicesVector indices_;
    FloatAttributeVec vertices_;
    FloatAttributeVec normals_;
    FloatAttributeVec tangens_;
    FloatAttributeVec textureCoords_;
    FloatAttributeVec* heights_;
};
}  // WBLoader
} // namespace GameEngine
