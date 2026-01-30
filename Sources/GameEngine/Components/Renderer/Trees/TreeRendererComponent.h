#pragma once
#include <Types.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ClusterData.h"
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
#include "Tree.h"

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

    File treeModel;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_INT(leafTextureAtlasSize)
        FIELD_FILE(treeModel)
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
    const GraphicsApi::ID& GetClusterShaderBufferId() const;
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
    void CreateClusterSsbo();
    void UpdateBoundingBox();
    void GenerateFileNameIfNeeded();

private:
    void ReleaseModels();
    void DeleteShaderBuffers();

private:
    Tree tree;

    std::vector<vec3> instancesPositions_;

    std::unique_ptr<ShaderStorageVectorBufferObject<LeafSSBO>> leafsSsbo_;
    std::unique_ptr<ShaderStorageVectorBufferObject<ClusterData>> clustersSsbo_;
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
