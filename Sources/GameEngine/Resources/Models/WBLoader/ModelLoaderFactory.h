#pragma once
#include "IModelLoaderFactory.h"
#include "AbstractLoader.h"

namespace GameEngine
{
class ITextureLoader;
class ModelLoaderFactory : public IModelLoaderFactory
{
public:
    ModelLoaderFactory(ITextureLoader&);
    LoadersVector createLoaders() const override;

private:
    ITextureLoader& textureloader;
};
}  // namespace GameEngine
