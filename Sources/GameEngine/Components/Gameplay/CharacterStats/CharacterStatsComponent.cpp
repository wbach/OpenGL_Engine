#include "CharacterStatsComponent.h"

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
CharacterStatsComponent::CharacterStatsComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<CharacterStatsComponent>(), componentContext, gameObject)
{
}
CharacterStatsComponent::~CharacterStatsComponent()
{
}
void CharacterStatsComponent::CleanUp()
{
}
void CharacterStatsComponent::Reload()
{
}
void CharacterStatsComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, [this]() { initGui(); });

    RegisterFunction(FunctionType::OnStart,
                     [this]()
                     {
                         componentContext_.inputManager_.SubscribeOnKeyDown(Input::GameAction::CHARACTER_VIEW,
                                                                            [this]()
                                                                            {
                                                                                if (not mainWindow)
                                                                                    return;

                                                                                if (mainWindow->isActive())
                                                                                {
                                                                                    hide();
                                                                                }
                                                                                else
                                                                                {
                                                                                    show();
                                                                                }
                                                                            });
                     });
}
void CharacterStatsComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<CharacterStatsComponent>(componentContext, gameObject);
        component->read(input);

        ::Read(input.getChild(GUI_FILE), component->guiFile);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<CharacterStatsComponent>(), func);
}
void CharacterStatsComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(GUI_FILE), guiFile);
}
void CharacterStatsComponent::initGui()
{
    if (mainWindow)
    {
        LOG_DEBUG << "Already initialized";
        return;
    }

    const std::string layerName{"CharacterViewer"};
    GUI::ElementReader reader(componentContext_.guiManager_, componentContext_.guiElementFactory_);

    if (reader.read(guiFile, layerName))
    {
        auto layer = componentContext_.guiManager_.getLayer(layerName);

        mainWindow = GUI::getTypedElement<GUI::Window>(layer, "MainWindow");

        if (auto exitButton = GUI::getTypedElement<GUI::Button>(layer, "Exit"))
        {
            exitButton->setOnClick([this]() { hide(); });
        }

        if (mainWindow)
        {
            mainWindow->activate(false);
        }

        updateGui();
    }
    else
    {
        LOG_WARN << "Character viewer init gui error";
    }
}
void CharacterStatsComponent::updateGui()
{
}
void CharacterStatsComponent::show()
{
    mainWindow->activate(true);

    componentContext_.inputManager_.SetReleativeMouseMode(false);
    componentContext_.inputManager_.ShowCursor(true);

    if (auto mainCamera = componentContext_.scene_.GetCameraManager().GetMainCamera())
    {
        mainCamera->Lock();
        updateGui();
    }
}
void CharacterStatsComponent::hide()
{
    mainWindow->activate(false);

    componentContext_.inputManager_.SetReleativeMouseMode(true);
    componentContext_.inputManager_.ShowCursor(false);

    if (auto mainCamera = componentContext_.scene_.GetCameraManager().GetMainCamera())
    {
        mainCamera->Unlock();
    }
}
}  // namespace Components
}  // namespace GameEngine
