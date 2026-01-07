#pragma once
#include <memory>
#include <vector>

#include "Model.h"
#include "AbstractLoader.h"
#include "GameEngine/Resources/File.h"
#include "IModelLoaderFactory.h"
#include "LoadingParameters.h"

namespace GameEngine
{
class ITextureLoader;

class LoaderManager
{
public:
    LoaderManager(std::unique_ptr<IModelLoaderFactory>);
    std::unique_ptr<Model> Load(const File&, const LoadingParameters& = DEFAULT_LOADING_PARAMETERS);

private:
    AbstractLoader* GetLoader(const File&);

private:
    std::unique_ptr<IModelLoaderFactory> factory_;
    LoadersVector loaders_;
};
}  // namespace GameEngine
