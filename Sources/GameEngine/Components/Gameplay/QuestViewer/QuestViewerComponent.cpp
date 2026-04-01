#include "QuestViewerComponent.h"

#include <Input/InputManager.h>
#include <Utils/TreeNode.h>

#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/GUI/IGuiElementFactory.h"
#include "GameEngine/Renderers/GUI/Layout/HorizontalLayout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
namespace Components
{
QuestViewerComponent::QuestViewerComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<QuestViewerComponent>(), componentContext, gameObject)
{
}
QuestViewerComponent::~QuestViewerComponent()
{
}
void QuestViewerComponent::CleanUp()
{
}
void QuestViewerComponent::Reload()
{
}
void QuestViewerComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart,
                     [this]()
                     {
                         componentContext_.inputManager_.SubscribeOnKeyDown(
                             Input::GameAction::QUEST_VIEW,
                             [this]()
                             {
                                 if (mainWindow)
                                 {
                                     componentContext_.guiManager_.Remove(*mainWindow);
                                     mainWindow = nullptr;
                                 }
                                 else
                                 {
                                     createWindow();
                                 }
                             });
                     });
}
void QuestViewerComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<QuestViewerComponent>(componentContext, gameObject);
        component->read(input);
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<QuestViewerComponent>(), func);
}
void QuestViewerComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);
}
void QuestViewerComponent::createWindow()
{
    const auto& quests = componentContext_.scene_.getEngineContext()->GetQuestManager().getQuests();

    auto& guiFactory  = componentContext_.guiElementFactory_;
    auto mainWndowPtr = guiFactory.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, vec2{0.5, 0.5}, vec2{1, 1});
    mainWindow        = mainWndowPtr.get();
    auto mainLayout   = guiFactory.CreateVerticalLayout();
    mainLayout->AutoHideElements(false);

    auto title = guiFactory.CreateGuiText("Quests");
   // title->setRenderMode(GuiTextElement::RenderMode::NATIVE);
    title->SetLocalScale( vec2(0.5, 0.05));
    mainLayout->AddChild(std::move(title));

    // ************** LEFT
    auto leftLayout = guiFactory.CreateHorizontalLayout();
    leftLayout->SetLocalScale(vec2ui(0.2, 0.5));
    auto innerOfleftLayout = guiFactory.CreateVerticalLayout();

    for (const auto& quest : quests)
    {
        auto questName = guiFactory.CreateGuiText(quest.name);
        //questName->setRenderMode(GuiTextElement::RenderMode::NATIVE);
        questName->SetLocalScale( vec2(0.5, 0.05));
        innerOfleftLayout->AddChild(std::move(questName));
    }

    leftLayout->AddChild(std::move(innerOfleftLayout));
    mainLayout->AddChild(std::move(leftLayout));

    // ************* RIGHT
    auto rightLayout = guiFactory.CreateHorizontalLayout();
    rightLayout->SetLocalScale(vec2ui(0.2, 0.5));
    auto innerOfRightLayout = guiFactory.CreateVerticalLayout();

    currentQuest = &quests.front();

    for (const auto& note : currentQuest->descriptionNotes)
    {
        auto noteTxt = guiFactory.CreateGuiText(note);
        //noteTxt->setRenderMode(GuiTextElement::RenderMode::NATIVE);
        noteTxt->SetLocalScale( vec2(0.5, 0.05));
        innerOfRightLayout->AddChild(std::move(noteTxt));
    }

    rightLayout->AddChild(std::move(innerOfRightLayout));
    mainLayout->AddChild(std::move(rightLayout));

    mainWndowPtr->AddChild(std::move(mainLayout));
    componentContext_.guiManager_.Add(std::move(mainWndowPtr));
}
}  // namespace Components
}  // namespace GameEngine
