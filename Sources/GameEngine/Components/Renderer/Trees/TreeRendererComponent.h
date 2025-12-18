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
    File leafPositionsFile;
    File modelLod1;
    File modelLod2;
    File modelLod3;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_FLOAT(leafScale)
        FIELD_INT(leafTextureAtlasSize)
        FIELD_INT(leafTextureIndex)
        FIELD_MATERIAL(leafMaterial)
        FIELD_MATERIAL(trunkMaterial)
        FIELD_FILE(leafPositionsFile)
        FIELD_FILE(modelLod1)
        FIELD_FILE(modelLod2)
        FIELD_FILE(modelLod3)
    END_FIELDS()
    // clang-format on

public:
    TreeRendererComponent(ComponentContext&, GameObject&);
    TreeRendererComponent& SetGeneratedModel(Model*, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetModel(const File&, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetInstancesPositions(const std::vector<vec3>&);
    TreeRendererComponent& SetLeafPosition(const std::vector<Leaf>&);
    TreeRendererComponent& SetLeafMaterial(const Material&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    const ModelWrapper& GetLeafModel() const;
    const ModelWrapper& GetModel() const;
    uint32 GetInstancesSize() const;
    const std::vector<vec3>& GetInstancesPositions() const;

    const GraphicsApi::ID& GetPerObjectUpdateId() const;
    const GraphicsApi::ID& GetPerInstancesBufferId() const;

    const BoundingBox& GetWorldBoundingBox() const;

private:
    void Awake();
    void UnSubscribe();
    void CreatePerObjectUpdateBuffer();
    void CreatePerInstancesBuffer();
    void UpdateBoundingBox();
    void CreateLeafModel();

private:
    void ReleaseModels();
    void DeleteShaderBuffers();

private:
    std::vector<Leaf> leafPositions;
    ModelWrapper model;
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
