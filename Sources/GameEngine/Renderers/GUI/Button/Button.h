#pragma once
#include <Utils/Time/Stopwatch.h>

#include <functional>

#include "GameEngine/Renderers/GUI/Element.h"
#include "GameEngine/Renderers/GUI/Theme.h"
#include "Types.h"

namespace GameEngine
{
namespace GUI
{
class Text;
class Sprite;

using OnClick = std::function<void()>;

class ENGINE_API Button : public Element
{
    enum class State
    {
        Normal,
        Hover,
        Active
    };

public:
    Button();

    void update() override;
    void onMouseClick(const vec2&, KeyCodes::Type) override;
    void onMouseEnter() override;
    void onMouseLeave() override;

    void setTextScale(float);
    void setText(std::unique_ptr<Text>);
    void setBackground(std::unique_ptr<Sprite>);
    void setOnHover(std::unique_ptr<Sprite>);
    void setOnActive(std::unique_ptr<Sprite>);
    void setOnClick(OnClick);

    void setBackground(const Color&);
    void setOnHover(const Color&);
    void setOnActive(const Color&);
    void setBackgroundText(const Color&);
    void setOnHoverText(const Color&);
    void setOnActiveText(const Color&);

    float getTextScale() const;
    const std::string& getText() const;
    Text* getTextElement() const;
    Sprite* getBackgroundSprite() const;
    Sprite* getOnHoverSprite() const;
    Sprite* getOnActiveSpirte() const;

    const Color& getBackgroundColor() const;
    const Color& getActiveColor() const;
    const Color& getOnHoverColor() const;

    void refreshSelf() override;

    void highlight();
    void toneDown();
    void executeAction();

    void setTheme(const Theme::Button&);

private:
    void applyState(State);
    void applyNormalState();
    void applyHoverState();
    void applyActiveState();
    void accept(IElementVisitor&) override;

private:
    OnClick onClick_;

    float textScale{1.f};

    std::unique_ptr<Text> text_;
    std::unique_ptr<Sprite> backgroundSprite;
    std::unique_ptr<Sprite> onHoverSprite;
    std::unique_ptr<Sprite> onActiveSprite;

    Theme::Button theme;

    Utils::Stopwatch activeTimer_;
    std::optional<uint32> subscribtion_;
    std::string actionName_;
    State currentState_;
};
}  // namespace GUI
}  // namespace GameEngine
