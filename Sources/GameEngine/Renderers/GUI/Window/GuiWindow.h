#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GuiWindowStyle.h"
#include "Input/InputManager.h"

namespace GameEngine
{
const float GUI_WINDOW_BAR_HEIGHT{0.05f};

class ENGINE_API GuiWindowElement : public GuiElement
{
public:
    GuiWindowElement(GuiWindowStyle, Input::InputManager&);
    ~GuiWindowElement() override;
    virtual void Update() override;
    void CheckCollisionPoint();
    GuiWindowStyle GetStyle() const;
    void SetStyle(GuiWindowStyle);
    void SetBackground(std::unique_ptr<GuiElement>);
    void SetBar(std::unique_ptr<GuiElement>);

public:
    void Show(bool) override;
    void Show() override;
    void Hide() override;
    void ShowPartial(uint32 depth) override;

private:
    Input::InputManager& inputManager_;
    GuiWindowStyle style_;

    std::unique_ptr<GuiElement> background_;
    std::unique_ptr<GuiElement> bar_;

public:
    static GuiElementTypes type;
    std::optional<vec2> collisionPoint_;
};
}  // namespace GameEngine
