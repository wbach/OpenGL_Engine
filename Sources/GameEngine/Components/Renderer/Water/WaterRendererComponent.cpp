#include "WaterRendererComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"

namespace GameEngine
{
namespace Components
{
const uint32 WATER_SCALE = 4000;

ComponentsType WaterRendererComponent::type = ComponentsType::Water;

WaterRendererComponent::WaterRendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(WaterRendererComponent::type, componentContext, gameObject)
    , position_(0, -75, 0)
    , scale_(WATER_SCALE, WATER_SCALE, 1)
{
}
void WaterRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&WaterRendererComponent::Subscribe, this));
}
const vec3& WaterRendererComponent::GetPosition() const
{
    return position_;
}
const vec3& WaterRendererComponent::GetScale() const
{
    return scale_;
}
void WaterRendererComponent::Subscribe()
{
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void WaterRendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
}  // namespace Components
}  // namespace GameEngine