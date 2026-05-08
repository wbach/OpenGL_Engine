#include "CustomMaterialData.h"

#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
CustomMaterialData::CustomMaterialData(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& loader,
                                       const Material& material)
    : material{material}
    , loader{loader}
{
    CreateBufferObject(graphicsApi);
}

CustomMaterialData::~CustomMaterialData()
{
    loader.AddObjectToRelease(std::move(perMeshBuffer));
}

void CustomMaterialData::CreateBufferObject(GraphicsApi::IGraphicsApi& graphicsApi)
{
    perMeshBuffer = std::make_unique<ShaderBufferObject<PerMeshObject>>(createPerMeshObject(material), graphicsApi,
                                                                        PER_MESH_OBJECT_BIND_LOCATION);
    loader.AddObjectToGpuLoadingPass(*perMeshBuffer);
}

GraphicsApi::ID CustomMaterialData::GetBufferId() const
{
    return perMeshBuffer->GetGraphicsObjectId();
}
}  // namespace Components
}  // namespace GameEngine
