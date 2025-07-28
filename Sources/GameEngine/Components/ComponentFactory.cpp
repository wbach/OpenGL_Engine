#include "ComponentFactory.h"

#include <Logger/Log.h>

#include "ComponentsReadFunctions.h"
#include "IComponent.h"

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
        auto iter = ReadFunctions::instance().componentsReadFunctions.find(componentName);
        if (iter != ReadFunctions::instance().componentsReadFunctions.end())
            return iter->second(context_, node, gameObject);

        WARNING_LOG("Read function not find for component name : " + componentName);
    }
    else
    {
        ERROR_LOG("Component type not found in component node attribute.");
    }

    return nullptr;
}
}  // namespace Components
}  // namespace GameEngine
