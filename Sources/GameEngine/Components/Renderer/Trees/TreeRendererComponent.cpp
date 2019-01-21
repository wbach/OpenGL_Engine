#include "TreeRendererComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType TreeRendererComponent::type = ComponentsType::TreeRenderer;

TreeRendererComponent::TreeRendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::TreeRenderer, componentContext, gameObject)
{
}
void TreeRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&TreeRendererComponent::Subscribe, this));
}
TreeRendererComponent& TreeRendererComponent::SetPositions(const std::vector<vec3>& positions, const vec2ui& size2d)
{
    positions_ = positions;
    if (size2d.x == 0 || size2d.y == 0)
    {
        auto s  = sqrt(positions.size());
        size2d_ = vec2ui(static_cast<uint32>(std::floor(s)));
    }
    else
    {
        size2d_ = size2d;
    }

    return *this;
}
TreeRendererComponent& TreeRendererComponent::SetBottomModel(const std::string& filename, GameEngine::LevelOfDetail i)
{
    if (filename.empty())
        return *this;

    bottomFilenames_.insert({ filename, i });

    auto model = GameEngine::LoadModel(&componentContext_.resourceManager_, filename);
    bottom_.Add(model, i);

    return *this;
}
TreeRendererComponent& TreeRendererComponent::SetTopModel(const std::string& filename, GameEngine::LevelOfDetail i)
{
    if (filename.empty())
        return *this;

    topFilenames_.insert({ filename, i });

    auto model = GameEngine::LoadModel(&componentContext_.resourceManager_, filename);
    top_.Add(model, i);

    return *this;
}
void TreeRendererComponent::Subscribe()
{
    if (not positions_.empty())
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
    }
}
void TreeRendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
}  // namespace Components
}  // namespace GameEngine
