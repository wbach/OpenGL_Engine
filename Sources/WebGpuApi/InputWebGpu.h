#pragma once

#include "Input/InputManager.h"

namespace GraphicsApi::WebGpu
{
class InputWebGpu : public Input::InputManager
{
public:
    InputWebGpu() = default;

    bool GetKey(KeyCodes::Type) override;
    bool GetMouseKey(KeyCodes::Type) override;
    void SetReleativeMouseMode(bool) override;
    vec2i CalcualteMouseMove() override;
    vec2i GetPixelMousePosition() override;
    vec2 GetMousePosition() override;
    void ClearKeyBuffer() override;
    void SetCursorPosition(int, int) override;
    void GetPressedKeys() override;
    void ShowCursor(bool) override;
    void ApplyPendingChanges() override;

protected:
    KeyCodes::Type ConvertCode(uint32) const override
    {
        return {};
    }
    bool IsKeyUpEventType(uint32) const override
    {
        return false;
    }
    bool IsKeyDownEventType(uint32) const override
    {
        return false;
    }
};

}  // namespace GraphicsApi::WebGpu
