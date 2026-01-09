#pragma once
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "Leaf.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class TreeRendererComponent : public BaseComponent
{
public:
    float leafScale;
    int leafTextureAtlasSize;
    int leafTextureIndex;
    File leafMaterial;
    File trunkMaterial;
    File leafModelFileLod1;
    File leafModelFileLod2;
    File leafModelFileLod3;
    File trunkModelLod1;
    File trunkModelLod2;
    File trunkModelLod3;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_FLOAT(leafScale)
        FIELD_INT(leafTextureAtlasSize)
        FIELD_INT(leafTextureIndex)
        FIELD_MATERIAL(leafMaterial)
        FIELD_MATERIAL(trunkMaterial)
        FIELD_FILE(leafModelFileLod1)
        FIELD_FILE(leafModelFileLod2)
        FIELD_FILE(leafModelFileLod3)
        FIELD_FILE(trunkModelLod1)
        FIELD_FILE(trunkModelLod2)
        FIELD_FILE(trunkModelLod3)
    END_FIELDS()
    // clang-format on

public:
    TreeRendererComponent(ComponentContext&, GameObject&);
    TreeRendererComponent& SetGeneratedTrunkModel(Model*, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetGeneratedLeafModel(Model*, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetTrunkModel(const File&, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetLeafModel(const File&, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetInstancesPositions(const std::vector<vec3>&);
    TreeRendererComponent& SetLeafMaterial(const Material&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void AddLeafClusterTexture(Material&, GraphicsApi::ID);

    const ModelWrapper& GetLeafModel() const;
    const ModelWrapper& GetTrunkModel() const;
    uint32 GetInstancesSize() const;
    const std::vector<vec3>& GetInstancesPositions() const;

    const GraphicsApi::ID& GetPerObjectUpdateId() const;
    const GraphicsApi::ID& GetPerInstancesBufferId() const;

    const BoundingBox& GetWorldBoundingBox() const;

    float windTime = 0.0f;

private:
    void Awake();
    void UnSubscribe();
    void CreatePerObjectUpdateBuffer();
    void CreatePerInstancesBuffer();
    void UpdateBoundingBox();

private:
    void ReleaseModels();
    void DeleteShaderBuffers();

private:
    ModelWrapper trunkModel;
    ModelWrapper leafModel;

    std::vector<vec3> instancesPositions_;

    std::unique_ptr<BufferObject<PerObjectUpdate>> perObjectUpdateBuffer_;
    std::unique_ptr<BufferObject<PerInstances>> perInstances_;
    bool isSubsribed_;

    std::optional<IdType> worldTransformSub_;

    BoundingBox wolrdModelBoundingBox;
    std::optional<Material> leafMaterialTmp;

    std::unordered_map<LevelOfDetail, Model*> generatedModels;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
