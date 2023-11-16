#include "Menu.h"

#include <algorithm>

#include "GameEngine/Renderers/GUI/Button/GuiButton.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/Layout/HorizontalLayout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"

namespace GameEngine
{
namespace
{
const vec2 defaultRootButtonSize{0.05f, 0.8f};
const vec2 defaultElementButtonSize{0.8f, 0.05f};
}
GuiElementTypes Menu::type = GuiElementTypes::HorizontalLayout;

Menu::Menu(GuiElementFactory& factory)
    : GuiElement(type)
    , rootLabel_("menu_root")
    , rootScale_{1.0f, 0.03}
    , rootPosition_{0.5, 1.f}
    , factory_{factory}
    , rootLayout_{}
{
    auto layout = factory_.CreateHorizontalLayout();
    rootLayout_ = layout.get();
    rootLayout_->SetAlgin(GameEngine::HorizontalLayout::Algin::LEFT);
    rootLayout_->SetLabel(rootLabel_);
    rootLayout_->SetScreenScale(rootScale_);
    rootLayout_->SetScreenPostion(rootPosition_);
    AddChild(std::move(layout));
}
void Menu::Update()
{
}
void Menu::AddRootElement(const std::string& label)
{
    VerticalLayout* vLayoutPtr{nullptr};

    auto vlayoutIter = roots_.find(label);
    if (vlayoutIter == roots_.end())
    {
        auto vLayout = factory_.CreateVerticalLayout();
        vLayout->SetLabel(label);
        vLayout->SetScreenScale({0.05, 0.8f});
        vLayout->Deactivate();
        //vLayout->SetAlgin(GameEngine::Layout::Algin::LEFT);
        //vLayout->EnableFixedSize();
        vLayoutPtr = vLayout.get();
        AddChild(std::move(vLayout));
    }
    else
    {
        vLayoutPtr = vlayoutIter->second;
    }

    auto action = [this, vLayoutPtr](GuiElement& button)
    {
        HideAllMenus();
        vLayoutPtr->Activate();
        vLayoutPtr->SetScreenPostion(button.GetScreenPosition() - vec2(0.f, vLayoutPtr->GetScreenScale().y / 2.f  ));
    };
    auto button = factory_.CreateGuiButton(label, action);
    button->SetScreenScale(defaultRootButtonSize);
    rootLayout_->AddChild(std::move(button));
}
void Menu::Add(const std::string& parentLabel, const std::string& text, const std::function<void(GuiElement&)> action)
{
    auto parentIter = std::find_if(children_.begin(), children_.end(),
                                   [&parentLabel](const auto& child) { return parentLabel == child->GetLabel(); });

    if (parentIter != children_.end())
    {
        auto button = factory_.CreateGuiButton(text, action);
        button->SetScreenScale(defaultElementButtonSize);
        (**parentIter).AddChild(std::move(button));
    }
}
void Menu::HideAllMenus()
{
    std::for_each(children_.begin(), children_.end(),
                  [this](auto& child)
                  {
                      if (child->GetLabel() != rootLabel_)
                          child->Deactivate();
                  });
}
}  // namespace GameEngine
