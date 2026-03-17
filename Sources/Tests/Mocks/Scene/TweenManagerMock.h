#pragma once
#include <GameEngine/Scene/ITweenManager.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class TweenManagerMock : public ITweenManager
{
public:
    MOCK_METHOD(void, Update, (float deltaTime), (override));
    MOCK_METHOD(void, Add,
                (GameObject & gameObject, const TweenTransform& target, float duration, EaseType ease,
                 std::function<void()> callback),
                (override));
    MOCK_METHOD(void, Remove, (const GameObject& gameObject), (override));
};
}  // namespace GameEngine