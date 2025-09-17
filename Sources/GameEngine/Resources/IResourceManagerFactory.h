#pragma once
#include <memory>

namespace GameEngine
{
class IResourceManager;

class IResourceManagerFactory
{
public:
    virtual ~IResourceManagerFactory() = default;
    virtual std::unique_ptr<IResourceManager> create() = 0;
};
}  // namespace GameEngine