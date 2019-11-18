#include "TreeView.h"
#include <Logger/Log.h>
#include <algorithm>
#include "GameEngine/Renderers/GUI/Button/GuiButton.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/Layout/HorizontalLayout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"

namespace GameEngine
{
namespace
{
const std::string EXPAND_TEXT         = " > ";
const std::string COLLAPSE_TEXT       = " v ";
const std::string EXAPND_BUTTON_LABEL = "expandButton";

}  // namespace
GuiElementTypes TreeView::type = GuiElementTypes::TreeView;

TreeView::TreeView(GuiElementFactory& factory, std::function<void(GuiElement&)> action, const vec2ui& windowSize)
    : GuiElement(type, windowSize)
    , elementfactory_(factory)
    , selectedItemAction_{action}
    , idPool_{0}
    , indentSize_{0.1f}
{
    CreateRootLayout();
}

uint32 TreeView::Add(const std::string& str, std::optional<uint32> parent)
{
    adedElements_.push_back({ idPool_, str, parent});

    auto node = CreateNode(str);
    elementMap_.insert({idPool_, node.vLayout.get()});

    if (not parent)
    {
        MoveNodeToLayout(*rootLayout_, node);
    }
    else
    {
        if (elementMap_.count(*parent) > 0)
        {
            auto h = node.hLayout.get();
            auto v = node.vLayout.get();
            MoveNodeToLayout(*elementMap_.at(*parent), node);
            h->Hide();
            v->Hide();
        }
        else
        {
            ERROR_LOG("Parent with id : " + std::to_string(*parent) + " not found");
        }
    }
    return idPool_++;
}

const AddedElementCommands& TreeView::GetAddedElementCommands() const
{
    return adedElements_;
}

const VerticalLayout& TreeView::GetRootLayout() const
{
    return *rootLayout_;
}

const std::string TreeView::GetActionName() const
{
    return actionName_;
}

void TreeView::SetActionName(const std::string& actionName)
{
    actionName_ = actionName;
}

void TreeView::CreateRootLayout()
{
    auto verticalLayout = elementfactory_.CreateVerticalLayout();
    verticalLayout->SetScale(vec2(0.1, 0.5));
    rootLayout_ = verticalLayout.get();
    rootLayout_->SetLabel(CreateLabel("root"));
    rootLayout_->EnableFixedSize();
    AddChild(std::move(verticalLayout));
}

void TreeView::MoveNodeToLayout(VerticalLayout& layout, Node& node)
{
    layout.AddChild(std::move(node.hLayout));
    layout.AddChild(std::move(node.vLayout));
}

Node TreeView::CreateNode(const std::string& text)
{
    auto childrenVerticalLayout = elementfactory_.CreateVerticalLayout();
    childrenVerticalLayout->SetLabel(CreateLabel(std::to_string(childrenVerticalLayout->GetId())));
    childrenVerticalLayout->SetXOffset(indentSize_);
    childrenVerticalLayout->EnableFixedSize();

    Node node;
    node.hLayout = CreateHorizonalLayout(text, childrenVerticalLayout.get());
    node.vLayout = std::move(childrenVerticalLayout);
    return node;
}
std::unique_ptr<HorizontalLayout> TreeView::CreateHorizonalLayout(const std::string& text,
                                                                  VerticalLayout* childrenLayout)
{
    auto horizontalLayout = elementfactory_.CreateHorizontalLayout();

    horizontalLayout->SetScale(vec2(0.1, 0.05));

    // Roll button
    {
        OnClick action = [childrenLayout](GuiElement& element) {
            if (not childrenLayout)
                return;

            auto button = static_cast<GuiButtonElement*>(&element);

            if (childrenLayout->IsShow())
            {
                childrenLayout->Hide();
                button->GetText()->SetText(EXPAND_TEXT);
            }
            else
            {
                childrenLayout->ShowPartial(0);

                for (auto& child : childrenLayout->GetChildren())
                {
                    if (child->GetType() == GuiElementTypes::HorizontalLayout)
                    {
                        child->Show();

                        const auto& grandChildren = child->GetChildren();
                        auto expandButton =
                            std::find_if(grandChildren.begin(), grandChildren.end(),
                                         [](const auto& child) { return child->GetLabel() == EXAPND_BUTTON_LABEL; });

                        if (expandButton != grandChildren.end())
                        {
                            static_cast<GuiButtonElement*>(expandButton->get())->GetText()->SetText(EXPAND_TEXT);
                        }
                    }
                }
                button->GetText()->SetText(COLLAPSE_TEXT);
            }
        };

        auto button = elementfactory_.CreateGuiButton(EXPAND_TEXT, action);
        button->SetLabel(EXAPND_BUTTON_LABEL);
        horizontalLayout->AddChild(std::move(button));
    }

    auto button = elementfactory_.CreateGuiButton(text, selectedItemAction_);
    button->SetLabel(std::to_string(idPool_));
    horizontalLayout->AddChild(std::move(button));
    return horizontalLayout;
}

std::string TreeView::CreateLabel(const std::string& name)
{
    return "TreeView_" + std::to_string(GetId()) + "_" + name;
}
}  // namespace GameEngine
