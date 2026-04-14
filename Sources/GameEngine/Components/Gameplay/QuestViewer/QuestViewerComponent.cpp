#include "QuestViewerComponent.h"

#include <Input/InputManager.h>
#include <Utils/TreeNodeWriteFunctions.h>
#include <Utils/TreeNodeReadFunctions.h>

#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Renderers/GUI/IElementFactory.h"
#include "GameEngine/Renderers/GUI/Layout/HorizontalLayout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Manager.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
#include "GameEngine/Renderers/GUI/Transform.h"
#include "GameEngine/Renderers/GUI/Window/Window.h"
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
    // RegisterFunction(FunctionType::OnStart,
    //                  [this]()
    //                  {
    //                      componentContext_.inputManager_.SubscribeOnKeyDown(
    //                          Input::GameAction::QUEST_VIEW,
    //                          [this]()
    //                          {
    //                              if (mainWindow)
    //                              {
    //                                  componentContext_.guiManager_.remove(*mainWindow);
    //                                  mainWindow = nullptr;
    //                              }
    //                              else
    //                              {
    //                                  createWindow();
    //                              }
    //                          });
    //                  });
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
    auto mainWndowPtr = guiFactory.createWindow(GUI::WindowStyle::BACKGROUND_ONLY);
    mainWndowPtr->setTransform(GUI::Transform{.position = vec2{0.5, 0.5}, .scale = vec2{1, 1}});
    mainWindow      = mainWndowPtr.get();
    auto mainLayout = guiFactory.createVerticalLayout();
    mainLayout->autoHideElements(false);

    auto title = guiFactory.createText("Quests");
    // title->setRenderMode(GuiTextElement::RenderMode::NATIVE);
    title->setLocalScale(vec2(0.5, 0.05));
    mainLayout->addChild(std::move(title));

    // ************** LEFT
    auto leftLayout = guiFactory.createHorizontalLayout();
    leftLayout->setLocalScale(vec2ui(0.2, 0.5));
    auto innerOfleftLayout = guiFactory.createVerticalLayout();

    for (const auto& quest : quests)
    {
        auto questName = guiFactory.createText(quest.name);
        // questName->setRenderMode(GuiTextElement::RenderMode::NATIVE);
        questName->setLocalScale(vec2(0.5, 0.05));
        innerOfleftLayout->addChild(std::move(questName));
    }

    leftLayout->addChild(std::move(innerOfleftLayout));
    mainLayout->addChild(std::move(leftLayout));

    // ************* RIGHT
    auto rightLayout = guiFactory.createHorizontalLayout();
    rightLayout->setLocalScale(vec2ui(0.2, 0.5));
    auto innerOfRightLayout = guiFactory.createVerticalLayout();

    currentQuest = &quests.front();

    for (const auto& note : currentQuest->descriptionNotes)
    {
        auto noteTxt = guiFactory.createText(note);
        // noteTxt->setRenderMode(GuiTextElement::RenderMode::NATIVE);
        noteTxt->setLocalScale(vec2(0.5, 0.05));
        innerOfRightLayout->addChild(std::move(noteTxt));
    }

    rightLayout->addChild(std::move(innerOfRightLayout));
    mainLayout->addChild(std::move(rightLayout));

    mainWndowPtr->addChild(std::move(mainLayout));
    componentContext_.guiManager_.add(std::move(mainWndowPtr));
}
}  // namespace Components
}  // namespace GameEngine
