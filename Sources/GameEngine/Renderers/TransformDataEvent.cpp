#include "TransformDataEvent.h"

#include <Common/Transform.h>

#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"

namespace GameEngine
{
TransformDataEvent::TransformDataEvent(Components::RendererComponent& renderComponent)
    : renderComponent_{renderComponent}
{
}
void TransformDataEvent::Execute()
{
    renderComponent_.UpdateBuffers();
}
}  // namespace GameEngine
