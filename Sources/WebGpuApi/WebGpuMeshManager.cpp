#include "WebGpuMeshManager.h"

namespace GraphicsApi::WebGpu
{
ID WebGpuMeshManager::CreateMesh(const MeshRawData& meshData, RenderType renderType)
{
    const uint32 meshId = nextMeshId_++;
    meshes_[meshId] = meshData;
    renderTypes_[meshId] = renderType;
    return meshId;
}

void WebGpuMeshManager::UpdateMesh(uint32 id, const MeshRawData& meshData, const std::set<VertexBufferObjects>& dirtyObjects)
{
    auto it = meshes_.find(id);
    if (it == meshes_.end())
    {
        return;
    }

    it->second = meshData;
    (void)dirtyObjects;
}

void WebGpuMeshManager::DeleteMesh(uint32 id)
{
    meshes_.erase(id);
    renderTypes_.erase(id);
}

const MeshRawData* WebGpuMeshManager::GetMesh(uint32 id) const
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
    const auto it = renderTypes_.find(id);
    if (it == renderTypes_.end())
    {
        return std::nullopt;
    }

    return it->second;
}

}  // namespace GraphicsApi::WebGpu
