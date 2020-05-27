#pragma once
#include "IGpuObject.h"

namespace GameEngine
{
class GpuObject : public IGpuObject
{
public:
    GpuObject();
    void GpuLoadingPass() override = 0;
    void UpdateGpuPass() override;
    void ReleaseGpuPass() override = 0;
    uint64 GetGpuObjectId() const override;
    const GraphicsApi::ID& GetGraphicsObjectId() const override;

protected:
    std::optional<uint32> graphicsObjectId_;

private:
    static uint64 s_Id_;
    uint64 id_;
};
}  // namespace GameEngine
