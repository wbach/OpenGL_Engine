#include "LoaderManager.h"

#include <Logger/Log.h>

#include "AbstractLoader.h"

namespace GameEngine
{
LoaderManager::LoaderManager(std::unique_ptr<IModelLoaderFactory> factory)
    : factory_(std::move(factory))
    , loaders_(factory_->createLoaders())
{
}

std::unique_ptr<Model> LoaderManager::Load(const File& file, const LoadingParameters& loadingParameters)
{
    auto loaderPtr = GetLoader(file);

    if (loaderPtr == nullptr)
    {
        LOG_ERROR << "Try parse unkonwn file extension : " << file.GetExtension();
        return nullptr;
    }

    auto isSuccessfullyParsed = loaderPtr->Parse(file, loadingParameters);
    if (not isSuccessfullyParsed)
    {
        LOG_ERROR << "Parse failed. File=" << file;
        return nullptr;
    }

    auto result = loaderPtr->Create();
    if (result)
    {
        result->SetFile(file);

        auto trianglesCount = 0u;
        for (const auto& mesh : result->GetMeshes())
        {
            trianglesCount += (not mesh.GetCMeshDataRef().indices_.empty())
                                  ? (mesh.GetCMeshDataRef().indices_.size() / 3)
                                  : (mesh.GetCMeshDataRef().positions_.size() / 3 / 3);
        }

        LOG_ERROR << "File  loaded: " << file << " Meshes count : " << result->GetMeshes().size()
                  << ", Total triangles count : " << trianglesCount;
    }
    else
    {
        LOG_ERROR << "File  creation error: " << file;
    }
    return result;
}

AbstractLoader* LoaderManager::GetLoader(const File& file)
{
    for (auto& loader : loaders_)
    {
        if (loader->CheckExtension(file))
            return loader.get();
    }
    return nullptr;
}
}  // namespace GameEngine
