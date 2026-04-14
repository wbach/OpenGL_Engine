#pragma once
#include <Input/InputManager.h>

#include <memory>
#include <optional>
#include <vector>

#include "GameEngine/Renderers/GUI/Element.h"
#include "WindowStyle.h"

namespace GameEngine
{
namespace GUI
{
class Sprite;
inline const float GUI_WINDOW_BAR_HEIGHT{0.05f};

class ENGINE_API Window : public Element
{
public:
    Window(WindowStyle);

    void onMouseClick(const vec2&, KeyCodes::Type) override;
    void onMouseClickRelease(KeyCodes::Type) override;
    void onMousePositionUpdate(const vec2&) override;

    void update() override;
    WindowStyle getStyle() const;
    void setStyle(WindowStyle);
    void setBackground(std::unique_ptr<Sprite>);
    void setBackground(const Color&);
    void setBar(std::unique_ptr<Element>);

private:
    void accept(IElementVisitor&) override;

private:
    WindowStyle style_;

    std::unique_ptr<Sprite> background_;
    std::unique_ptr<Element> bar_;

    std::optional<vec2> mousePosition;
    std::optional<vec2> mouseReferencePosition;
};
}  // namespace GUI
}  // namespace GameEngine
