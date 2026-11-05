#pragma once

#include <optional>
#include <set>
#include <unordered_map>

#include "GraphicsApi/GraphicsApiDef.h"
#include "GraphicsApi/MeshRawData.h"
#include "GraphicsApi/RenderType.h"

namespace GraphicsApi::WebGpu
{
class WebGpuMeshManager
{
public:
    ID CreateMesh(const MeshRawData& meshData, RenderType renderType);
    void UpdateMesh(uint32 id, const MeshRawData& meshData, const std::set<VertexBufferObjects>& dirtyObjects);
    void DeleteMesh(uint32 id);

    const MeshRawData* GetMesh(uint32 id) const;
    std::optional<RenderType> GetRenderType(uint32 id) const;

private:
    uint32 nextMeshId_ = 1;
    std::unordered_map<uint32, MeshRawData> meshes_;
    std::unordered_map<uint32, RenderType> renderTypes_;
};

}  // namespace GraphicsApi::WebGpu
