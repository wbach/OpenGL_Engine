#include "ComponentFactory.h"

#include <Logger/Log.h>
#include <Utils/TreeNode.h>

#include "ComponentsReadFunctions.h"
#include "IComponent.h"
#include "UnknownExternalComponent.h"

namespace GameEngine
{
namespace Components
{
ComponentFactory::ComponentFactory(Scene& scene, ISceneManager& sceneManager, ComponentController& componentController,
                                   GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader, Time& time,
                                   Input::InputManager& input, IResourceManager& resourceManager,
                                   Renderer::RenderersManager& rendererManager, CameraWrapper& camera,
                                   Physics::IPhysicsApi& physicsApi, GuiElementFactory& guiElementFactory,
                                   Utils::Time::TimerService& timerService)
    : context_(scene, sceneManager, graphicsApi, gpuResourceLoader, time, input, camera, physicsApi, resourceManager,
               rendererManager, componentController, guiElementFactory, timerService)
{
}
std::unique_ptr<IComponent> ComponentFactory::Create(const TreeNode& node, GameObject& gameObject)
{
    const std::string CSTR_TYPE = "type";
    auto componentName          = node.getAttributeValue(CSTR_TYPE);
    if (not componentName.empty())
    {
        if (auto creationFunc = ReadFunctions::instance().get(componentName))
            return creationFunc(context_, node, gameObject);

        LOG_WARN << "Read function not find for component name : " << componentName;
    }
    else
    {
        LOG_ERROR << "Component type not found in component node attribute.";
    }

    if (auto creationFunc = ReadFunctions::instance().get(GetComponentType<UnknownExternalComponent>().name))
        return creationFunc(context_, node, gameObject);

    return nullptr;
}
}  // namespace Components
}  // namespace GameEngine
