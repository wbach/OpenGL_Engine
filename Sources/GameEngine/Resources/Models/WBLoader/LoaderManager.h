#pragma once
#include <memory>
#include <vector>
#include "../Model.h"

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
    std::unique_ptr<Model> Load(const std::string& file_name);
    ~LoaderManager();

private:
    WBLoader::AbstractLoader* GetLoader(const std::string& extension);

private:
    typedef std::vector<std::unique_ptr<WBLoader::AbstractLoader>> LoadersVector;
    LoadersVector loaders_;
    ITextureLoader& textureloader;
};
}  // namespace GameEngine
