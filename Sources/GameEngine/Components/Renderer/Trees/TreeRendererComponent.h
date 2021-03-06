#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
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
    TreeRendererComponent(ComponentContext&, GameObject&);
    TreeRendererComponent& SetTopModel(const std::string& filename,
                                       GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetBottomModel(const std::string& filename,
                                          GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetPositions(const std::vector<vec3>& positions, const vec2ui& size2d = vec2ui(0, 0));

    void CleanUp() override;
    void ReqisterFunctions() override;

    inline ModelWrapper& GetTopModelWrapper();
    inline ModelWrapper& GetBottomModelWrapper();
    inline std::vector<vec3>& GetPositions();
    inline uint32 GetInstancesSize() const;
    inline const vec2ui& GetPositionSize2d() const;
    inline const std::unordered_map<std::string, LevelOfDetail>& GetTopFileNames() const;
    inline const std::unordered_map<std::string, LevelOfDetail>& GetBottomFileNames() const;
    inline const std::vector<vec3>& GetCPositions() const;
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
    std::unordered_map<std::string, LevelOfDetail> topFilenames_;
    std::unordered_map<std::string, LevelOfDetail> bottomFilenames_;
    ModelWrapper top_;
    ModelWrapper bottom_;
    vec2ui size2d_;
    std::vector<vec3> positions_;

    std::unique_ptr<BufferObject<PerObjectUpdate>> perObjectUpdateBuffer_;
    std::unique_ptr<BufferObject<PerInstances>> perInstances_;
    bool isSubsribed_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};

ModelWrapper& TreeRendererComponent::GetTopModelWrapper()
{
    return top_;
}
ModelWrapper& TreeRendererComponent::GetBottomModelWrapper()
{
    return bottom_;
}
std::vector<vec3>& TreeRendererComponent::GetPositions()
{
    return positions_;
}
inline uint32 TreeRendererComponent::GetInstancesSize() const
{
    return static_cast<uint32>(positions_.size());
}
const vec2ui& TreeRendererComponent::GetPositionSize2d() const
{
    return size2d_;
}
inline const std::unordered_map<std::string, LevelOfDetail>& TreeRendererComponent::GetTopFileNames() const
{
    return topFilenames_;
}
inline const std::unordered_map<std::string, LevelOfDetail>& TreeRendererComponent::GetBottomFileNames() const
{
    return bottomFilenames_;
}
const std::vector<vec3>& TreeRendererComponent::GetCPositions() const
{
    return positions_;
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
