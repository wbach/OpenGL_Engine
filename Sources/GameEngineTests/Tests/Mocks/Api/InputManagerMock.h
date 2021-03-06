#pragma once
#include <Input/InputManager.h>
#include <gmock/gmock.h>

namespace Input
{
class InputManagerMock : public InputManager
{
public:
    MOCK_METHOD1(GetKey, bool(KeyCodes::Type));
    MOCK_METHOD1(GetMouseKey, bool(KeyCodes::Type));
    MOCK_METHOD1(SetReleativeMouseMode, void(bool));
    MOCK_METHOD0(CalcualteMouseMove, vec2i());
    MOCK_METHOD0(GetPixelMousePosition, vec2i());
    MOCK_METHOD0(GetMousePosition, vec2());

    MOCK_METHOD2(SetCursorPosition, void(int, int));
    MOCK_METHOD2(SetKeyToBuffer, void(int, bool));
    MOCK_METHOD0(ClearKeyBuffer, void());
    MOCK_METHOD0(GetPressedKeys, void());
    MOCK_METHOD1(ShowCursor, void(bool));

    MOCK_CONST_METHOD1(ConvertCode, KeyCodes::Type(uint32));
    MOCK_CONST_METHOD1(IsKeyUpEventType, bool(uint32));
    MOCK_CONST_METHOD1(IsKeyDownEventType, bool(uint32));
};
} // namespace Input
