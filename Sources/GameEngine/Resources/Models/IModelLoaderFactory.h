#pragma once
#include <memory>
#include <vector>

namespace GameEngine
{
class AbstractLoader;

using LoadersVector = std::vector<std::unique_ptr<AbstractLoader>>;

class IModelLoaderFactory
{
public:
    virtual ~IModelLoaderFactory()        = default;
    virtual LoadersVector createLoaders() const = 0;
};
}  // namespace GameEngine