#include "VulkanIdPool.h"

namespace GraphicsApi::Vulkan
{
VulkanIdPool::VulkanIdPool()
    : IdPool(1, IdPool::IdReusePolicy::OnExhaustion)
{
}
IdType VulkanIdPool::AccquireNewId(VulkanObjectType type)
{
    std::lock_guard<std::mutex> lock(mutex_);

    const auto id = getIdUnlocked();

    try
    {
        createdObjectIds.emplace(id, type);
    }
    catch (...)
    {
        releaseIdUnlocked(id);
        throw;
    }

    return id;
}
std::optional<VulkanObjectType> VulkanIdPool::GetObjectType(IdType id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = createdObjectIds.find(id);
    if (iter != createdObjectIds.end())
    {
        return iter->second;
    }
    return std::nullopt;
}
void VulkanIdPool::ReleaseId(IdType id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = createdObjectIds.find(id);
    if (iter == createdObjectIds.end())
    {
        return;
    }
    releaseIdUnlocked(id);
    createdObjectIds.erase(iter);
}
}  // namespace GraphicsApi::Vulkan
