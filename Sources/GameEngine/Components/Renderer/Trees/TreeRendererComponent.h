#pragma once
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderStorageVectorBufferObject.h"
#include "GraphicsApi/GraphicsApiDef.h"
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
    File leafMaterialFile;
    File trunkMaterial;
    File leafsFileLod1;
    File leafsFileLod2;
    File leafsFileLod3;
    File leafsBilboards;
    File trunkModelLod1;
    File trunkModelLod2;
    File trunkModelLod3;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_FLOAT(leafScale)
        FIELD_INT(leafTextureAtlasSize)
        FIELD_INT(leafTextureIndex)
        FIELD_MATERIAL(leafMaterialFile)
        FIELD_MATERIAL(trunkMaterial)
        FIELD_FILE(leafsFileLod1)
        // FIELD_FILE(leafsFileLod2)
        // FIELD_FILE(leafsFileLod3)
        FIELD_FILE(leafsBilboards)
        FIELD_FILE(trunkModelLod1)
        FIELD_FILE(trunkModelLod2)
        FIELD_FILE(trunkModelLod3)
    END_FIELDS()
    // clang-format on

public:
    TreeRendererComponent(ComponentContext&, GameObject&);
    TreeRendererComponent& SetGeneratedTrunkModel(Model*, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetLeafBilboardsModel(Model*);
    TreeRendererComponent& SetTrunkModel(const File&, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetInstancesPositions(const std::vector<vec3>&);
    TreeRendererComponent& SetLeafMaterial(const Material&);

    void UpdateLeafsSsbo(std::vector<LeafSSBO>&&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void AddLeafClusterTexture(Material&, GraphicsApi::ID);

    const Model* GetLeafBilboardsModel() const;
    const ModelWrapper& GetTrunkModel() const;
    uint32 GetInstancesSize() const;
    const std::vector<vec3>& GetInstancesPositions() const;

    const GraphicsApi::ID& GetPerObjectUpdateId() const;
    const GraphicsApi::ID& GetPerInstancesBufferId() const;
    const GraphicsApi::ID& GetLeafsShaderBufferId() const;
    const BoundingBox& GetWorldBoundingBox() const;

    const Material& GetLeafMaterial() const;
    float windTime = 0.0f;

    size_t GetLeafsCount() const;

    void SetClusterTextures(const ClusterTextures&);
    void SetTreeClusters(const TreeClusters&);

    const ClusterTextures& getClusterTextures() const;
    const TreeClusters& getTreeClusters() const;

private:
    void Awake();
    void UnSubscribe();
    void CreatePerObjectUpdateBuffer();
    void CreatePerInstancesBuffer();
    void CreateLeafsSsbo();
    void UpdateBoundingBox();

private:
    void ReleaseModels();
    void DeleteShaderBuffers();

private:
    ModelWrapper trunkModel;
    Model* leafBilboardsModel{nullptr};
    ClusterTextures clusterTextures;
    TreeClusters treeClusters;

    std::vector<vec3> instancesPositions_;

    Material leafMaterial;
    std::unique_ptr<ShaderStorageVectorBufferObject<LeafSSBO>> leafsSsbo_;
    std::unique_ptr<ShaderBufferObject<PerObjectUpdate>> perObjectUpdateBuffer_;
    std::unique_ptr<ShaderBufferObject<PerInstances>> perInstances_;
    bool isSubsribed_;

    std::optional<IdType> worldTransformSub_;

    BoundingBox wolrdModelBoundingBox;

    std::unordered_map<LevelOfDetail, Model*> generatedModels;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
