#pragma once
#include "../AbstractLoader.h"
#include "Types.h"

class CTextureLoader;

namespace WBLoader
{
class TerrainMeshLoader : public AbstractLoader
{
public:
    TerrainMeshLoader(CTextureLoader& textureLodaer);
    virtual void ParseFile(const std::string& filename) override;
    virtual bool CheckExtension(const std::string& filename) override;
    virtual std::unique_ptr<CModel> Create() override;

private:
    void CreateTerrainVertexes(uint16 x_start, uint16 y_start, uint16 width, uint16 height);
    vec3 CalculateNormalMap(uint16 x, uint16 z);
    float GetHeight(uint16 x, uint16 y) const;
    void CreateMesh();
    void Clear();

private:
    uint16 heightMapResolution_;
    std::vector<uint16> indices_;
    std::vector<float> vertices_;
    std::vector<float> normals_;
    std::vector<float> tangens_;
    std::vector<float> textureCoords_;
    std::vector<float>* heights_;
};
}  // WBLoader
