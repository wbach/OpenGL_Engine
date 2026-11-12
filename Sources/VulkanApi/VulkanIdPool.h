#pragma once
#include <Utils/IdPool.h>

#include <mutex>
#include <optional>
#include <unordered_map>

#include "VulkanObjectType.h"

namespace GraphicsApi::Vulkan
{
class VulkanIdPool : protected Utils::IdPool
{
public:
    VulkanIdPool();

    IdType AccquireNewId(VulkanObjectType);
    std::optional<VulkanObjectType> GetObjectType(IdType);
    void ReleaseId(IdType);

private:
    std::mutex mutex_;

    std::unordered_map<IdType, VulkanObjectType> createdObjectIds;
};
}  // namespace GraphicsApi::Vulkan