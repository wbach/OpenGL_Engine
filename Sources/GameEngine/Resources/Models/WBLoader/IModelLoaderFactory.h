#pragma once
#include <memory>
#include <vector>

namespace GameEngine
{
namespace WBLoader
{
class AbstractLoader;
}

using LoadersVector = std::vector<std::unique_ptr<WBLoader::AbstractLoader>>;

class IModelLoaderFactory
{
public:
    virtual ~IModelLoaderFactory()        = default;
    virtual LoadersVector createLoaders() const = 0;
};
}  // namespace GameEngine