#pragma once
#include <memory>
#include <optional>
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
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class TreeRendererComponent : public BaseComponent
{
public:
    File leafMaterial;
    File trunkMaterial;
    File leafPositionsFile;
    File modelLod1;
    File modelLod2;
    File modelLod3;

    // clang-format off
    BEGIN_FIELDS()
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

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    const ModelWrapper& GetModel() const;
    uint32 GetInstancesSize() const;
    const std::vector<vec3>& GetInstancesPositions() const;

    const GraphicsApi::ID& GetPerObjectUpdateId() const;
    const GraphicsApi::ID& GetPerInstancesBufferId() const;

    const BoundingBox& GetWorldBoundingBox() const;

private:
    void Subscribe();
    void UnSubscribe();
    void CreatePerObjectUpdateBuffer();
    void CreatePerInstancesBuffer();
    void UpdateBoundingBox();

private:
    void ReleaseModels();
    void DeleteShaderBuffers();

private:
    std::vector<vec3> leafPositions;
    ModelWrapper model;

    std::vector<vec3> instancesPositions_;

    std::unique_ptr<BufferObject<PerObjectUpdate>> perObjectUpdateBuffer_;
    std::unique_ptr<BufferObject<PerInstances>> perInstances_;
    bool isSubsribed_;

    std::optional<IdType> worldTransformSub_;

    BoundingBox wolrdModelBoundingBox;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
