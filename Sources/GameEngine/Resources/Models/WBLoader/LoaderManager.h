#pragma once
#include <memory>
#include <vector>
#include "../Model.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
class ITextureLoader;

namespace WBLoader
{
class AbstractLoader;
}

class LoaderManager
{
public:
    LoaderManager(ITextureLoader& textureloader);
    std::unique_ptr<Model> Load(const File&);
    ~LoaderManager();

private:
    WBLoader::AbstractLoader* GetLoader(const File&);

private:
    typedef std::vector<std::unique_ptr<WBLoader::AbstractLoader>> LoadersVector;
    LoadersVector loaders_;
    ITextureLoader& textureloader;
};
}  // namespace GameEngine
