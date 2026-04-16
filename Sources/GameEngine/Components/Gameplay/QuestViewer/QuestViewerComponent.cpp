#include "QuestViewerComponent.h"

#include <Input/InputManager.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <utility>

#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Renderers/GUI/Button/Button.h"
#include "GameEngine/Renderers/GUI/ElementReader.h"
#include "GameEngine/Renderers/GUI/ElementWriter.h"
#include "GameEngine/Renderers/GUI/IElementFactory.h"
#include "GameEngine/Renderers/GUI/IGuiRenderer.h"
#include "GameEngine/Renderers/GUI/Layout/HorizontalLayout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Manager.h"
#include "GameEngine/Renderers/GUI/Text/MultiLineText.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
#include "GameEngine/Renderers/GUI/Transform.h"
#include "GameEngine/Renderers/GUI/Window/Window.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
namespace
{
constexpr char GUI_FILE[]{"guiLayoutFile"};
}
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
    RegisterFunction(FunctionType::Awake, [this]() { initGui(); });

    RegisterFunction(FunctionType::OnStart,
                     [this]()
                     {
                         componentContext_.inputManager_.SubscribeOnKeyDown(
                             Input::GameAction::QUEST_VIEW,
                             [this]()
                             {
                                 if (not mainWindow)
                                     return;

                                 mainWindow->activate(not mainWindow->isActive());
                                 componentContext_.inputManager_.SetReleativeMouseMode(not mainWindow->isActive());
                                 componentContext_.inputManager_.ShowCursor(mainWindow->isActive());

                                 auto mainCamera = componentContext_.scene_.GetCameraManager().GetMainCamera();
                                 if (mainWindow->isActive())
                                 {
                                     if (mainCamera)
                                     {
                                         mainCamera->Lock();
                                     }

                                     updateGui();
                                 }
                                 else
                                 {
                                     if (mainCamera)
                                     {
                                         mainCamera->Unlock();
                                     }
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

        ::Read(input.getChild(GUI_FILE), component->guiFile);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<QuestViewerComponent>(), func);
}
void QuestViewerComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(GUI_FILE), guiFile);
}
void QuestViewerComponent::initGui()
{
    if (mainWindow)
    {
        LOG_DEBUG << "Already initialized";
        return;
    }

    const std::string layerName{"QuestViewer"};
    GUI::ElementReader reader(componentContext_.guiManager_, componentContext_.guiElementFactory_);

    if (reader.read(guiFile, layerName))
    {
        auto layer = componentContext_.guiManager_.getLayer(layerName);

        mainWindow = GUI::getTypedElement<GUI::Window>(layer, "MainWindow");

        contentLayout = GUI::getTypedElement<GUI::VerticalLayout>(layer, "ContentLayout");
        questLayout   = GUI::getTypedElement<GUI::VerticalLayout>(layer, "QuestsLayout");

        if (auto contentText = GUI::getTypedElement<GUI::MultiLineText>(layer, "ContentItem"))
        {
            GUI::ElementWriter::write(questDescriptionText, *contentText);
        }

        if (auto questItem = GUI::getTypedElement<GUI::Button>(layer, "QuestItem"))
        {
            GUI::ElementWriter::write(questButtonTemplate, *questItem);
        }

        if (mainWindow)
        {
            mainWindow->activate(false);
        }

        updateGui();
    }
    else
    {
        LOG_WARN << "Quest viewer init gui error";
    }
}
void QuestViewerComponent::updateGui()
{
    if (not questLayout)
        return;

    questLayout->removeAll();
    contentLayout->removeAll();

    const auto& quests = componentContext_.scene_.getEngineContext()->GetQuestManager().getQuests();
    for (const auto& quest : quests)
    {
        GUI::ElementReader reader(componentContext_.guiManager_, componentContext_.guiElementFactory_);
        auto button = reader.readButton(questButtonTemplate);
        if (auto text = button->getTextElement())
        {
            text->text.text = quest.name;
        }

        button->setOnClick([this, &quest]() { updateContent(quest); });
        questLayout->addChild(std::move(button));
    }
}
void QuestViewerComponent::updateContent(const Quest& quest)
{
    if (contentLayout)
    {
        contentLayout->removeAll();

        GUI::ElementReader reader(componentContext_.guiManager_, componentContext_.guiElementFactory_);
        for (auto& description : quest.descriptionNotes)
        {
            if (auto text = reader.readMultiLineText(questDescriptionText))
            {
                text->setText(description);
                contentLayout->addChild(std::move(text));
            }
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
