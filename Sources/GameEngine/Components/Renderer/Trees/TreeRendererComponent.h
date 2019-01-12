#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
namespace Components
{
class TreeRendererComponent : public BaseComponent
{
public:
    TreeRendererComponent(const ComponentContext& componentContext, GameObject& gameObject);
    TreeRendererComponent& SetTopModel(const std::string& filename, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetBottomModel(const std::string& filename, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    TreeRendererComponent& SetPositions(const std::vector<vec3>& positions, const vec2ui& size2d = vec2ui(0, 0));
    virtual void ReqisterFunctions() override;
    inline ModelWrapper& GetTopModelWrapper();
    inline ModelWrapper& GetBottomModelWrapper();
    inline std::vector<vec3>& GetPositions();
    inline const vec2ui GetPositionSize2d();

private:
    void Subscribe();
    void UnSubscribe();

private:
    ModelWrapper top_;
    ModelWrapper bottom_;
    vec2ui size2d_;
    std::vector<vec3> positions_;

public:
    static ComponentsType type;
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
const vec2ui TreeRendererComponent::GetPositionSize2d()
{
    return size2d_;
}
}  // namespace Components
}  // namespace GameEngine
