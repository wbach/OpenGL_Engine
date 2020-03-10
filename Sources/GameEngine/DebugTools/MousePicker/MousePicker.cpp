#include "MousePicker.h"

namespace GameEngine
{
MousePicker::MousePicker(CameraWrapper& camera, Projection& projection)
    : camera_(camera)
    , projection_(projection)
{
}
GameObject* MousePicker::SelectObject(std::vector<std::unique_ptr<GameObject>>&)
{
    return nullptr;
}
}  // namespace GameEngine