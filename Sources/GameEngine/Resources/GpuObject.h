#pragma once
#include "IGpuObject.h"

namespace GameEngine
{
class GpuObject : public IGpuObject
{
public:
    GpuObject();
    inline void GpuLoadingPass() override = 0;
    inline void ReleaseGpuPass() override = 0;
    inline uint64 GetGpuObjectId() const override;
    inline const GraphicsApi::ID& GetGraphicsObjectId() const override;

protected:
    std::optional<uint32> graphicsObjectId_;

private:
    static uint64 s_Id_;
    uint64 id_;
};

void GpuObject::GpuLoadingPass()
{
}

const GraphicsApi::ID& GpuObject::GetGraphicsObjectId() const
{
    return graphicsObjectId_;
}
void GpuObject::ReleaseGpuPass()
{
    graphicsObjectId_ = std::nullopt;
}
inline uint64 GpuObject::GetGpuObjectId() const
{
    return id_;
}
}  // namespace GameEngine
