#pragma once
#include "IGpuObject.h"
#include "Types.h"

namespace GameEngine
{
class GpuObject : public IGpuObject
{
public:
    GpuObject();
    ~GpuObject();

    GpuObject(const GpuObject&) = delete;
    GpuObject(GpuObject&&) noexcept;
    GpuObject& operator=(GpuObject const&) = delete;
    GpuObject& operator=(GpuObject&&) noexcept;

    void GpuLoadingPass() override = 0;
    void UpdateGpuPass() override;
    void ReleaseGpuPass() override = 0;
    IdType GetGpuObjectId() const override;
    const GraphicsApi::ID& GetGraphicsObjectId() const override;

protected:
    std::optional<uint32> graphicsObjectId_;

private:
    IdType id_;
};
}  // namespace GameEngine
