#include "WebGpuMeshManager.h"

#include <vector>

namespace GraphicsApi::WebGpu
{
namespace
{
std::vector<float> BuildInterleavedVertexBuffer(const MeshRawData& meshData)
{
    const auto vertexCount = meshData.positions_.size() / 3u;
    std::vector<float> vertices;
    vertices.reserve(vertexCount * 5u);

    for (uint32 i = 0; i < vertexCount; ++i)
    {
        const auto posOffset = i * 3u;
        vertices.push_back(meshData.positions_[posOffset]);
        vertices.push_back(meshData.positions_[posOffset + 1u]);
        vertices.push_back(meshData.positions_[posOffset + 2u]);

        const auto uvOffset = i * 2u;
        if (uvOffset + 1u < meshData.textCoords_.size())
        {
            vertices.push_back(meshData.textCoords_[uvOffset]);
            vertices.push_back(meshData.textCoords_[uvOffset + 1u]);
        }
        else
        {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    return vertices;
}
}  // namespace

void WebGpuMeshManager::SetDevice(WGPUDevice device, WGPUQueue queue)
{
    device_ = device;
    queue_ = queue;
}

ID WebGpuMeshManager::CreateMesh(const MeshRawData& meshData, RenderType renderType)
{
    const uint32 meshId = nextMeshId_++;
    WebGpuMeshResource resource{};
    resource.meshData = meshData;
    resource.renderType = renderType;
    resource.vertexCount = static_cast<uint32>(meshData.positions_.size() / 3u);
    resource.hasIndices = !meshData.indices_.empty();
    resource.indexCount = static_cast<uint32>(meshData.indices_.size());

    if (device_ && !meshData.positions_.empty())
    {
        if (!CreateVertexBuffer(resource))
        {
            return std::nullopt;
        }

        if (resource.hasIndices && !CreateIndexBuffer(resource))
        {
            ReleaseResource(resource);
            return std::nullopt;
        }
    }

    meshes_[meshId] = std::move(resource);
    return meshId;
}

void WebGpuMeshManager::UpdateMesh(uint32 id, const MeshRawData& meshData, const std::set<VertexBufferObjects>& dirtyObjects)
{
    auto it = meshes_.find(id);
    if (it == meshes_.end())
    {
        return;
    }

    it->second.meshData = meshData;
    it->second.vertexCount = static_cast<uint32>(meshData.positions_.size() / 3u);
    it->second.hasIndices = !meshData.indices_.empty();
    it->second.indexCount = static_cast<uint32>(meshData.indices_.size());

    if (device_)
    {
        ReleaseResource(it->second);
        if (!meshData.positions_.empty())
        {
            CreateVertexBuffer(it->second);
            if (it->second.hasIndices)
            {
                CreateIndexBuffer(it->second);
            }
        }
    }

    (void)dirtyObjects;
}

void WebGpuMeshManager::DeleteMesh(uint32 id)
{
    auto it = meshes_.find(id);
    if (it == meshes_.end())
    {
        return;
    }

    ReleaseResource(it->second);
    meshes_.erase(it);
}

const MeshRawData* WebGpuMeshManager::GetMesh(uint32 id) const
{
    const auto it = meshes_.find(id);
    if (it == meshes_.end())
    {
        return nullptr;
    }

    return &it->second.meshData;
}

const WebGpuMeshResource* WebGpuMeshManager::GetMeshResource(uint32 id) const
{
    const auto it = meshes_.find(id);
    if (it == meshes_.end())
    {
        return nullptr;
    }

    return &it->second;
}

std::optional<RenderType> WebGpuMeshManager::GetRenderType(uint32 id) const
{
    const auto it = meshes_.find(id);
    if (it == meshes_.end())
    {
        return std::nullopt;
    }

    return it->second.renderType;
}

void WebGpuMeshManager::ReleaseResource(WebGpuMeshResource& resource)
{
    if (resource.vertexBuffer)
    {
        wgpuBufferDestroy(resource.vertexBuffer);
        wgpuBufferRelease(resource.vertexBuffer);
        resource.vertexBuffer = nullptr;
    }

    if (resource.indexBuffer)
    {
        wgpuBufferDestroy(resource.indexBuffer);
        wgpuBufferRelease(resource.indexBuffer);
        resource.indexBuffer = nullptr;
    }
}

bool WebGpuMeshManager::CreateVertexBuffer(WebGpuMeshResource& resource)
{
    const auto vertices = BuildInterleavedVertexBuffer(resource.meshData);
    if (vertices.empty())
    {
        return true;
    }

    WGPUBufferDescriptor descriptor{};
    descriptor.label = WGPUStringView{"WebGPU Mesh Vertex Buffer", WGPU_STRLEN};
    descriptor.size = vertices.size() * sizeof(float);
    descriptor.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
    descriptor.mappedAtCreation = WGPU_FALSE;

    resource.vertexBuffer = wgpuDeviceCreateBuffer(device_, &descriptor);
    if (!resource.vertexBuffer)
    {
        return false;
    }

    wgpuQueueWriteBuffer(queue_, resource.vertexBuffer, 0, vertices.data(), descriptor.size);
    return true;
}

bool WebGpuMeshManager::CreateIndexBuffer(WebGpuMeshResource& resource)
{
    if (resource.meshData.indices_.empty())
    {
        return true;
    }

    WGPUBufferDescriptor descriptor{};
    descriptor.label = WGPUStringView{"WebGPU Mesh Index Buffer", WGPU_STRLEN};
    descriptor.size = resource.meshData.indices_.size() * sizeof(uint32_t);
    descriptor.usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst;
    descriptor.mappedAtCreation = WGPU_FALSE;

    resource.indexBuffer = wgpuDeviceCreateBuffer(device_, &descriptor);
    if (!resource.indexBuffer)
    {
        return false;
    }

    wgpuQueueWriteBuffer(queue_, resource.indexBuffer, 0, resource.meshData.indices_.data(), descriptor.size);
    return true;
}

}  // namespace GraphicsApi::WebGpu
