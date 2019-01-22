#pragma once
#include <list>
#include <string>
#include "../Model.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "MeshData.h"

namespace GameEngine
{
class ITextureLoader;

namespace WBLoader
{
class AbstractLoader
{
public:
    AbstractLoader(GraphicsApi::IGraphicsApi& graphicsApi, ITextureLoader& textureLoader);
    void Parse(const std::string& filename);
    virtual bool CheckExtension(const std::string& filename) = 0;
    virtual std::unique_ptr<Model> Create();

protected:
    virtual void ParseFile(const std::string& filename) = 0;
    std::list<WBLoader::Object> objects;
    std::unique_ptr<Model> CreateModel();
    std::unique_ptr<Model> CreateModelFromBin();

protected:
    ITextureLoader& textureLoader_;

private:
    void NormalizeMatrix(mat4& mat, float factor) const;
    float FindMaxFactor() const;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::string fileName_;
    std::string filePath_;
    bool loadedFromBin_;
};

typedef std::unique_ptr<WBLoader::AbstractLoader> AbstractLoaderPtr;
}  // WBLoader
}  // namespace GameEngine
