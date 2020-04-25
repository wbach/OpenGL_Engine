#pragma once
#include "IGpuObject.h"

namespace GameEngine
{
class GpuObject : public IGpuObject
{
public:
    inline void GpuLoadingPass() override;
    inline std::optional<uint32> GetGraphicsObjectId() const override;

protected:
    std::optional<uint32> graphicsObjectId_;
};

void GpuObject::GpuLoadingPass()
{
}

std::optional<uint32> GpuObject::GetGraphicsObjectId() const
{
    return graphicsObjectId_;
}
}  // namespace GameEngine
