#pragma once
#include "IGpuObject.h"

namespace GameEngine
{
class GpuObject : public IGpuObject
{
public:
    inline void GpuLoadingPass() override;
    inline std::optional<uint32> GetGraphicsObjectId() const override;
    inline void Reset();

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
void GpuObject::Reset()
{
    graphicsObjectId_ = std::nullopt;
}
}  // namespace GameEngine
