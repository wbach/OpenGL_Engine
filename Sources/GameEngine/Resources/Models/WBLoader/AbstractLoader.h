#pragma once
#include <list>
#include <string>
#include "../Model.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "MeshData.h"
#include "LoadingParameters.h"

namespace GameEngine
{
class ITextureLoader;

namespace WBLoader
{
class AbstractLoader
{
public:
    AbstractLoader(GraphicsApi::IGraphicsApi&, ITextureLoader&);
    virtual ~AbstractLoader() = default;
    virtual bool CheckExtension(const File&) = 0;

    void Parse(const File&, const LoadingParameters& = DEFAULT_LOADING_PARAMETERS);
    virtual std::unique_ptr<Model> Create();

protected:
    virtual void ParseFile(const File&) = 0;
    std::list<WBLoader::Object> objects;
    std::unique_ptr<Model> CreateModel();
    std::unique_ptr<Model> CreateModelFromBin();

protected:
    ITextureLoader& textureLoader_;

private:
    void NormalizeMatrix(mat4& mat, float factor) const;
    BoundingBox getModelBoundingBox() const;

protected:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    LoadingParameters loadingParameters_;
    std::string fileName_;
    std::string filePath_;
    bool loadedFromBin_;
};

typedef std::unique_ptr<WBLoader::AbstractLoader> AbstractLoaderPtr;
}  // WBLoader
}  // namespace GameEngine
