#pragma once
#include <memory>

#include "GameEngine/Components/Component.h"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/ShaderBufferObject.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
struct PerMeshObject;
class IGpuResourceLoader;

namespace Components
{
class CustomMaterialData
{
public:
    CustomMaterialData(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, const Material&);
    ~CustomMaterialData();

    GraphicsApi::ID GetBufferId() const;

public:
    Material material;

private:
    void CreateBufferObject(GraphicsApi::IGraphicsApi&);
    std::unique_ptr<ShaderBufferObject<PerMeshObject>> perMeshBuffer;

    IGpuResourceLoader& loader;
};
}  // namespace Components
}  // namespace GameEngine
