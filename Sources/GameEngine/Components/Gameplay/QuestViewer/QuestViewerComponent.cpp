#include "QuestViewerComponent.h"

#include <Input/InputManager.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <utility>

#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Renderers/GUI/Layer/DefaultLayers.h"
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
#include "Input/KeyCodes.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace
{
constexpr char GUI_FILE[]{"guiLayoutFile"};
}
namespace Components
{
REGISTER_COMPONENT(QuestViewerComponent)

QuestViewerComponent::QuestViewerComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
    , keySubManager(componentContext.inputManager_)
{
}
QuestViewerComponent::~QuestViewerComponent()
{
}
void QuestViewerComponent::CleanUp()
{
    keySubManager.UnsubscribeKeys();
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
                         keySubManager = componentContext_.inputManager_.SubscribeOnKeyDown(Input::GameAction::QUEST_VIEW,
                                                                            [this]()
                                                                            {
                                                                                if (not group)
                                                                                    return;

                                                                                if (group->isActive())
                                                                                {
                                                                                    hide();
                                                                                }
                                                                                else
                                                                                {
                                                                                    show();
                                                                                }
                                                                            });
                         keySubManager = componentContext_.inputManager_.SubscribeOnKeyDown(KeyCodes::ESCAPE, [this]() { hide(); });
                     });
}
void QuestViewerComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(GUI_FILE), guiFile);
}
void QuestViewerComponent::write(TreeNode& node) const
{
    ::write(node.addChild(GUI_FILE), guiFile);
}
void QuestViewerComponent::initGui()
{
    if (group)
    {
        LOG_DEBUG << "Already initialized";
        return;
    }

    GUI::ElementReader reader(componentContext_.guiManager_, componentContext_.guiElementFactory_);

    const auto& layerGroupName = GetTypeName();

    if (reader.read(guiFile, Layers::Panels, layerGroupName))
    {
        layer = componentContext_.guiManager_.getLayer(Layers::Panels);
        group = layer->getGroup(layerGroupName);

        if (auto exitButton = group->get<GUI::Button>("Exit"))
        {
            exitButton->setOnClick([this]() { hide(); });
        }

        contentLayout = group->get<GUI::VerticalLayout>("ContentLayout");
        questLayout   = group->get<GUI::VerticalLayout>("QuestsLayout");

        if (auto contentText = group->get<GUI::MultiLineText>("ContentItem"))
        {
            GUI::ElementWriter::write(questDescriptionText, *contentText);
        }

        if (auto questItem = group->get<GUI::Button>("QuestItem"))
        {
            GUI::ElementWriter::write(questButtonTemplate, *questItem);
        }

        group->activate(false);

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
    activeQuestButton = nullptr;

    const auto& quests = componentContext_.scene_.getEngineContext()->GetQuestManager().getQuests();

    int i = 0;
    for (const auto& quest : quests)
    {
        if (not quest.isActive)
        {
            continue;
        }
        GUI::ElementReader reader(componentContext_.guiManager_, componentContext_.guiElementFactory_);
        auto button = reader.readButton(questButtonTemplate);
        if (auto text = button->getTextElement())
        {
            text->text.text = quest.name;
        }

        if (i == 0)
        {
            button->setAsActive();
            activeQuestButton = button.get();
            updateContent(quest);
        }

        button->setOnClick(
            [this, &quest, ptr = button.get()]()
            {
                if (activeQuestButton)
                {
                    activeQuestButton->resetActiveState();
                }
                activeQuestButton = ptr;
                ptr->setAsActive();
                updateContent(quest);
            });
        questLayout->addChild(std::move(button));

        ++i;
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
void QuestViewerComponent::show()
{
    if (not group)
        return;

    group->activate(true);

    componentContext_.inputManager_.SetReleativeMouseMode(false);
    componentContext_.inputManager_.ShowCursor(true);

    if (auto mainCamera = componentContext_.scene_.GetCameraManager().GetMainCamera())
    {
        mainCamera->Lock();
        updateGui();
    }
}
void QuestViewerComponent::hide()
{
    if (not group or not layer)
        return;

    group->activate(false);
    if (layer->isActive())
    {
        LOG_DEBUG << "Other elements in layer " << Layers::Panels << " are active";
        return;
    }

    componentContext_.inputManager_.SetReleativeMouseMode(true);
    componentContext_.inputManager_.ShowCursor(false);

    if (auto mainCamera = componentContext_.scene_.GetCameraManager().GetMainCamera())
    {
        mainCamera->Unlock();
    }
}
}  // namespace Components
}  // namespace GameEngine
