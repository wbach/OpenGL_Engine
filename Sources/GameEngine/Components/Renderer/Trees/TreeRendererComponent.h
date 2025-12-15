#pragma once
#include <memory>
#include <vector>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

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
    inline uint32 GetInstancesSize() const;
    const std::vector<vec3>& GetInstancesPositions() const;

    inline const GraphicsApi::ID& GetPerObjectUpdateId() const;
    inline const GraphicsApi::ID& GetPerInstancesBufferId() const;

private:
    void Subscribe();
    void UnSubscribe();
    void CreatePerObjectUpdateBuffer();
    void CreatePerInstancesBuffer();

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

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};

uint32 TreeRendererComponent::GetInstancesSize() const
{
    return static_cast<uint32>(instancesPositions_.size());
}

inline const GraphicsApi::ID& TreeRendererComponent::GetPerObjectUpdateId() const
{
    return perObjectUpdateBuffer_->GetGraphicsObjectId();
}
inline const GraphicsApi::ID& TreeRendererComponent::GetPerInstancesBufferId() const
{
    return perInstances_->GetGraphicsObjectId();
}
}  // namespace Components
}  // namespace GameEngine
