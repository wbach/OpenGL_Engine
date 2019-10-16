#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/InputManager.h"
#include "GuiWindowStyle.h"

namespace GameEngine
{
class GuiWindowElement : public GuiElement
{
public:
    GuiWindowElement(GuiWindowStyle style, const vec2ui& windowSize, Input::InputManager& inputManager);
    ~GuiWindowElement() override;
    virtual void Update() override;
    void CheckCollisionPoint();
    GuiWindowStyle GetStyle() const;

private:


private:
    Input::InputManager& inputManager_;
    float titleBarSize_;
    uint32 inputSubscribtionKeyDown_;
    uint32 inputSubscribtionKeyUp_;
    GuiWindowStyle style_;

public:
    static GuiElementTypes type;
    std::optional<vec2> collisionPoint_;
};
}  // namespace GameEngine
