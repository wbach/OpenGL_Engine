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
}  // namespace Components

class TransformDataEvent : public IBufferDataUpdaterEvent
{
public:
    TransformDataEvent(Components::RendererComponent& renderComponent);
    virtual void Execute() override;

private:
    Components::RendererComponent& renderComponent_;
};
}  // namespace GameEngine