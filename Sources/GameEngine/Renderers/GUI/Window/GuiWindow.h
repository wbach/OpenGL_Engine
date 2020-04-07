#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GuiWindowStyle.h"
#include "Input/InputManager.h"

namespace GameEngine
{
const float GUI_WINDOW_BAR_HEIGHT{0.4f};

class GuiWindowElement : public GuiElement
{
public:
    GuiWindowElement(GuiWindowStyle style, const vec2ui& windowSize, Input::InputManager& inputManager);
    ~GuiWindowElement() override;
    virtual void Update() override;
    void CheckCollisionPoint();
    GuiWindowStyle GetStyle() const;
    void SetBackground(std::unique_ptr<GuiElement>);
    void SetBar(std::unique_ptr<GuiElement>);

public:
    void SetScale(const vec2& scale) override;
    void SetPostion(const vec2& position) override;
    void Show(bool) override;
    void Show() override;
    void Hide() override;

private:
    Input::InputManager& inputManager_;
    float titleBarSize_;
    uint32 inputSubscribtionKeyDown_;
    uint32 inputSubscribtionKeyUp_;
    GuiWindowStyle style_;

    std::unique_ptr<GuiElement> background_;
    std::unique_ptr<GuiElement> bar_;

public:
    static GuiElementTypes type;
    std::optional<vec2> collisionPoint_;
};
}  // namespace GameEngine
