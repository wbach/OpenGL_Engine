#pragma once

#include <optional>
#include <set>
#include <unordered_map>
#include <webgpu/webgpu.h>

#include "GraphicsApi/GraphicsApiDef.h"
#include "GraphicsApi/MeshRawData.h"
#include "GraphicsApi/RenderType.h"

namespace GraphicsApi::WebGpu
{
struct WebGpuMeshResource
{
    MeshRawData meshData;
    RenderType renderType{RenderType::TRIANGLES};
    WGPUBuffer vertexBuffer = nullptr;
    WGPUBuffer indexBuffer = nullptr;
    uint32 vertexCount = 0;
    uint32 indexCount = 0;
    uint32 vertexStride = 19u;
    bool hasIndices = false;
};

class WebGpuMeshManager
{
public:
    void SetDevice(WGPUDevice device, WGPUQueue queue);

    ID CreateMesh(const MeshRawData& meshData, RenderType renderType);
    void UpdateMesh(uint32 id, const MeshRawData& meshData, const std::set<VertexBufferObjects>& dirtyObjects);
    void DeleteMesh(uint32 id);

    const MeshRawData* GetMesh(uint32 id) const;
    const WebGpuMeshResource* GetMeshResource(uint32 id) const;
    std::optional<RenderType> GetRenderType(uint32 id) const;

private:
    void ReleaseResource(WebGpuMeshResource& resource);
    bool CreateVertexBuffer(WebGpuMeshResource& resource);
    bool CreateIndexBuffer(WebGpuMeshResource& resource);

private:
    uint32 nextMeshId_ = 1;
    WGPUDevice device_ = nullptr;
    WGPUQueue queue_ = nullptr;
    std::unordered_map<uint32, WebGpuMeshResource> meshes_;
};

}  // namespace GraphicsApi::WebGpu
