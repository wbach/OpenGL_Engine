#include "RendererComponent.hpp"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType RendererComponent::type = ComponentsType::Renderer;

RendererComponent::RendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Renderer, componentContext, gameObject)
    , textureIndex_(0)
{
}

RendererComponent::~RendererComponent()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}

void RendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&RendererComponent::Subscribe, this));
}
RendererComponent& RendererComponent::AddModel(const std::string& filename, GameEngine::LevelOfDetail lvl)
{
    if (filename.empty())
        return *this;

    filenames_.insert({filename, lvl});

    auto model = GameEngine::LoadModel(&componentContext_.resourceManager_, filename);
    model_.Add(model, lvl);

    return *this;
}
RendererComponent& RendererComponent::SetModel(const ModelWrapper& model)
{
    model_ = model;
    return *this;
}
RendererComponent& RendererComponent::SetTextureIndex(uint32_t index)
{
    textureIndex_ = index;
    return *this;
}
void RendererComponent::Subscribe()
{
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void RendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
}  // namespace Components
}  // namespace GameEngine