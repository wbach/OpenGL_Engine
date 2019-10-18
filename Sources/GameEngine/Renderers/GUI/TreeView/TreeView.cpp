#include "TreeView.h"
#include "GameEngine/Renderers/GUI/Button/GuiButton.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Layout/HorizontalLayout.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"

namespace GameEngine
{
GuiElementTypes TreeView::type = GuiElementTypes::TreeView;

TreeView::TreeView(GuiElementFactory& factory, std::function<void(const std::string&)> action, const vec2ui& windowSize)
    : GuiElement(type, windowSize)
    , elementfactory_(factory)
    , selectedItemAction_{action}
    , idPool_{0}
{
    auto verticalLayout = factory.CreateVerticalLayout();

    layout_ = verticalLayout.get();
    AddChild(std::move(verticalLayout));
}

uint32 TreeView::Add(const std::string& str, std::optional<uint32> parent)
{
    auto node = CreateNode(str);
    elementMap_.insert({ idPool_, node.get() });

    if (not parent)
    {
        layout_->AddChild(std::move(node));
    }
    else
    {
        if (elementMap_.count(*parent) > 0)
        {
            elementMap_.at(*parent)->AddChild(std::move(node));
        }
    }
    return idPool_++;
}
std::unique_ptr<HorizontalLayout> TreeView::CreateNode(const std::string & text)
{
    auto horizontalLayout = elementfactory_.CreateHorizontalLayout();
    auto horizontalLayoutPtr = horizontalLayout.get();

    // Roll button
    {
        OnClick action = [this, horizontalLayoutPtr](GuiElement& element) {
            for (auto& child : horizontalLayoutPtr->GetChildren())
            {
                if (child->IsShow())
                {
                    child->Hide();
                }
                else
                {
                    child->Show();
                }
            }
        };
        auto button = elementfactory_.CreateGuiButton(text, action);
        horizontalLayout->AddChild(std::move(button));
    }

    // Press button
    OnClick action = [this](GuiElement& element) {
        if (element.GetType() != GuiElementTypes::Button)
            return;
        auto button = static_cast<GuiButtonElement*>(&element);
        if (button->GetText())
        {
            selectedItemAction_(button->GetText()->GetText());
        }
    };
    auto button = elementfactory_.CreateGuiButton(text, action);
    return horizontalLayout;
}
}  // namespace GameEngine
