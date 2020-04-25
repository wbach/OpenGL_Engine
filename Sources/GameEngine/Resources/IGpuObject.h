#pragma once
#include <Types.h>
#include <optional>

namespace GameEngine
{
class IGpuObject
{
public:
    virtual ~IGpuObject() = default;

    virtual void GpuLoadingPass()                             = 0;
    virtual std::optional<uint32> GetGraphicsObjectId() const = 0;
};
}  // namespace GameEngine
