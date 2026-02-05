#pragma once
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Resources/Models/LoadingParameters.h"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectConstants.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "Types.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
class Mesh;
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

class RendererComponent : public BaseComponent
{
public:
    File fileName_LOD1;
    File fileName_LOD2;
    File fileName_LOD3;
    bool modelNormalization;
    bool meshOptimize;
    uint32_t textureIndex;
    MaterialsMap materials;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(fileName_LOD1)
        FIELD_FILE(fileName_LOD2)
        FIELD_FILE(fileName_LOD3)
        FIELD_BOOL(modelNormalization)
        FIELD_BOOL(meshOptimize)
        FIELD_UINT(textureIndex)
        FIELD_CONST_MAP_OF_MATERIALS(materials)
    END_FIELDS()
    // clang-format on

public:
    RendererComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;
    void Activate() override;
    void Deactivate() override;

    RendererComponent& AddModel(Model*, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    RendererComponent& AddModel(const std::filesystem::path&, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    RendererComponent& SetTextureIndex(uint32_t index);

    void useArmature(bool);
    void UpdateBuffers();  // Call on rendering thread

    inline ModelWrapper& GetModelWrapper();
    inline const ModelWrapper& GetModelWrapper() const;
    inline uint32_t GetTextureIndex() const;
    const GraphicsApi::ID& GetPerObjectUpdateBuffer(IdType) const;
    const GraphicsApi::ID& GetPerObjectConstantsBuffer(IdType) const;
    std::unordered_map<LevelOfDetail, File> GetFiles() const;
    const std::unordered_map<IdType, CustomMaterialData>& GetCustomMaterials() const;
    void AddCustomMaterial(const Material&);
    void AddCustomMaterial(const Mesh&, const Material&);
    void UpdateCustomMaterial(const Mesh&, const Material&);
    const BoundingBox& getWorldSpaceBoundingBox() const;

private:
    void init();
    void ClearShaderBuffers();
    void DeleteShaderBuffer(std::unique_ptr<GpuObject>);
    void Subscribe();
    void UnSubscribe();
    void ReserveBufferVectors(size_t);
    void CreateBuffers(Model&);
    void CreatePerObjectUpdateBuffer(const Mesh&);
    void CreatePerObjectConstantsBuffer(const Mesh&);
    void CreatePerMaterialBuffer(const Mesh&);
    void PrepareCustomMaterials(const Model&);
    void ReleaseCustomMaterials(const Model&);
    void CleanUpWithRestrictions(const std::set<Model*>& = {});
    void calculateWorldSpaceBoundingBox(const mat4&);

private:
    ModelWrapper model_;
    bool isSubscribed_;
    bool isInit_;
    LoadingParameters loadingParameters_;
    std::optional<uint32> worldTransformSub_;
    std::unordered_map<IdType, std::unique_ptr<ShaderBufferObject<PerObjectUpdate>>> perObjectUpdateBuffer_;
    std::unordered_map<IdType, std::unique_ptr<ShaderBufferObject<PerObjectConstants>>> perObjectConstantsBuffer_;

    std::unordered_map<IdType, CustomMaterialData> customMaterials;
    std::set<Model*> addedModels;
    BoundingBox boundingBox;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};

ModelWrapper& RendererComponent::GetModelWrapper()
{
    return model_;
}
const ModelWrapper& RendererComponent::GetModelWrapper() const
{
    return model_;
}
uint32_t RendererComponent::GetTextureIndex() const
{
    return textureIndex;
}
}  // namespace Components
}  // namespace GameEngine
