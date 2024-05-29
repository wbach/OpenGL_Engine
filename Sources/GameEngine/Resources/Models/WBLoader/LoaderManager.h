#pragma once
#include <memory>
#include <vector>
#include "../Model.h"
#include "GameEngine/Resources/File.h"
#include "AbstractLoader.h"
#include "LoadingParameters.h"

namespace GameEngine
{
class ITextureLoader;

class LoaderManager
{
public:
    LoaderManager(ITextureLoader&);
    std::unique_ptr<Model> Load(const File&, const LoadingParameters& = DEFAULT_LOADING_PARAMETERS);

private:
    WBLoader::AbstractLoader* GetLoader(const File&);

private:
    typedef std::vector<std::unique_ptr<WBLoader::AbstractLoader>> LoadersVector;
    LoadersVector loaders_;
};
}  // namespace GameEngine
