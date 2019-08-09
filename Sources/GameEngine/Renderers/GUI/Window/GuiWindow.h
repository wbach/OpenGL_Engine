#pragma once
#include <memory>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/InputManager.h"
#include <optional>

namespace GameEngine
{
class GuiWindowElement : public GuiElement
{
public:
    GuiWindowElement(const vec2ui& windowSize, Input::InputManager& inputManager);
    ~GuiWindowElement() override;
    void AddChild(GuiElement*);
    virtual void Update() override;
    virtual void SetScale(const vec2& scale);
    virtual void Show(bool);
    virtual void Show() override;
    virtual void Hide() override;
    virtual void SetZPosition(float z) override;

private:
    std::vector<GuiElement*> children_;
    Input::InputManager& inputManager_;
    float titleBarSize_;
    uint32 inputSubscribtionKeyDown_;
    uint32 inputSubscribtionKeyUp_;

public:
    static GuiElementTypes type;
    std::optional<vec2> collisionPoint_;
};
}  // namespace GameEngine
