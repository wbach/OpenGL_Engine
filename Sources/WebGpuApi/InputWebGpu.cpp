#include "InputWebGpu.h"

namespace GraphicsApi::WebGpu
{
bool InputWebGpu::GetKey(KeyCodes::Type)
{
    return false;
}
bool InputWebGpu::GetMouseKey(KeyCodes::Type)
{
    return false;
}
void InputWebGpu::SetReleativeMouseMode(bool)
{
}
vec2i InputWebGpu::CalcualteMouseMove()
{
    return {};
}
vec2i InputWebGpu::GetPixelMousePosition()
{
    return {};
}
vec2 InputWebGpu::GetMousePosition()
{
    return {};
}
void InputWebGpu::ClearKeyBuffer()
{
}
void InputWebGpu::SetCursorPosition(int, int)
{
}
void InputWebGpu::GetPressedKeys()
{
}
void InputWebGpu::ShowCursor(bool)
{
}
void InputWebGpu::ApplyPendingChanges()
{
}
}  // namespace GraphicsApi::WebGpu
