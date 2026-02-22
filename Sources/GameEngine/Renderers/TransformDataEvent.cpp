#include "TransformDataEvent.h"

#include <Common/Transform.h>

#include "GameEngine/Components/Animation/BowPoseUpdater.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
TransformDataEvent::TransformDataEvent(GameObject& gameObject)
    : gameObject{gameObject}
{
}
void TransformDataEvent::Execute()
{
    auto rendererComponents = gameObject.get().GetComponents<Components::RendererComponent>();
    for (auto rendererComponent : rendererComponents)
        rendererComponent->UpdateBuffers();
}
}  // namespace GameEngine
