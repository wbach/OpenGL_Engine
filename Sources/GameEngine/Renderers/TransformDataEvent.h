#pragma once
#include <Types.h>
#include <functional>

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

class TransformDataEvent
{
public:
    TransformDataEvent(Components::RendererComponent&);
    void Execute();

private:
    std::reference_wrapper<Components::RendererComponent> renderComponent_;
};
}  // namespace GameEngine
