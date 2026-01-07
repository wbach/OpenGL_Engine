#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <list>
#include <string>

#include "LoadingParameters.h"
#include "MeshData.h"
#include "Model.h"

namespace GameEngine
{
class ITextureLoader;

class AbstractLoader
{
public:
    AbstractLoader(GraphicsApi::IGraphicsApi&, ITextureLoader&);
    virtual ~AbstractLoader()                = default;
    virtual bool CheckExtension(const File&) = 0;

    bool Parse(const File&, const LoadingParameters& = DEFAULT_LOADING_PARAMETERS);
    virtual std::unique_ptr<Model> Create();

protected:
    virtual bool ParseFile(const File&) = 0;
    std::list<Object> objects;
    std::unique_ptr<Model> CreateModel();

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

typedef std::unique_ptr<AbstractLoader> AbstractLoaderPtr;
}  // namespace GameEngine
