#pragma once
#include <Types.h>
#include <functional>
#include "GameEngine/Objects/GameObject.h"

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
    TransformDataEvent(GameObject&);
    void Execute();

private:
    std::reference_wrapper<GameObject> gameObject;
};
}  // namespace GameEngine
