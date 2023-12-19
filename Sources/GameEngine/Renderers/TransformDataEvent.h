#pragma once
#include <Types.h>
#include "IBufferDataUpdaterEvent.h"

namespace common
{
class Transform;
}  // namespace common

namespace GameEngine
{
namespace Components
{
class RendererComponent;
class JointPoseUpdater;

}  // namespace Components

class TransformDataEvent : public IBufferDataUpdaterEvent
{
public:
    TransformDataEvent(Components::RendererComponent&, Components::JointPoseUpdater*);
    virtual void Execute() override;

private:
    Components::RendererComponent& renderComponent_;
    Components::JointPoseUpdater* jointPoseUpdater_;
};
}  // namespace GameEngine