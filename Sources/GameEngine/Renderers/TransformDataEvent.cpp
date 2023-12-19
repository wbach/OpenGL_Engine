#include "TransformDataEvent.h"

#include <Common/Transform.h>

#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"

namespace GameEngine
{
TransformDataEvent::TransformDataEvent(Components::RendererComponent& renderComponent,
                                       Components::JointPoseUpdater* jointPoseUpdater)
    : renderComponent_{renderComponent}
    , jointPoseUpdater_{jointPoseUpdater}
{
}
void TransformDataEvent::Execute()
{
    renderComponent_.UpdateBuffers();
    if (jointPoseUpdater_)
    {
        jointPoseUpdater_->updateGameObjectTransform();
    }
}
}  // namespace GameEngine
