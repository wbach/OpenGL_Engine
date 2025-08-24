#include "PauseMenuComponent.h"

#include <GameEngine/Components/ComponentsReadFunctions.h>
#include <Logger/Log.h>

#include <GameEngine/Scene/Scene.hpp>

#include "PauseMenuTheme.h"

namespace
{
const std::string COMPONENT_STR{"PauseMenu"};
const std::string CSTR_START_STATE{"startState"};
const std::string CSTR_LOGO_FILE{"logo"};
const std::string CSTR_BACKGROUND{"background"};
}  // namespace

PauseMenuComponent::PauseMenuComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(PauseMenuComponent).hash_code(), componentContext, gameObject)
    , pauseMenuImage("GUI/1200px-Avatar_The_Last_Airbender_logo.svg.png")
    , mainMenuBackground("GUI/204473.jpg")
    , startState{PauseMenu::State::MainMenu}
    , backgroundTexture{nullptr}
{
}

void PauseMenuComponent::CleanUp()
{
    pauseMenu.reset();
    if (backgroundTexture)
    {
        auto& guiElementFactory = componentContext_.guiElementFactory_;
        guiElementFactory.getManager().Remove(*backgroundTexture);
        backgroundTexture = nullptr;
    }
}

void PauseMenuComponent::ReqisterFunctions()
{
    DEBUG_LOG("Hello from PauseMenuComponent");
    RegisterFunction(FunctionType::Awake,
                     [this]()
                     {
                         if (pauseMenu)
                         {
                             LOG_DEBUG << "Menu already created. return";
                             return;
                         }
                         DEBUG_LOG("Create pause menu");
                         auto& guiElementFactory = componentContext_.guiElementFactory_;
                         guiElementFactory.SetTheme(getGuiTheme());

                         const auto& avaiableScenes = componentContext_.sceneManager_.GetAvaiableScenes();

                         pauseMenu = std::make_unique<PauseMenu>(pauseMenuImage, startState, componentContext_.scene_,
                                                                 guiElementFactory, avaiableScenes);

                         if (startState == PauseMenu::State::MainMenu)
                         {
                             auto background = guiElementFactory.CreateGuiTexture(mainMenuBackground.GetAbsoultePath());
                             background->SetZPosition(100.f);
                             backgroundTexture = background.get();
                             guiElementFactory.getManager().Add(std::move(background));
                             DEBUG_LOG("Background added");

                             pauseMenu->show();
                         }
                     });
}

void PauseMenuComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        DEBUG_LOG("Create PauseMenuComponent");
        auto component = std::make_unique<PauseMenuComponent>(componentContext, gameObject);
        if (auto maybeNode = node.getChild(CSTR_LOGO_FILE))
        {
            component->pauseMenuImage = maybeNode->value_;
        }
        if (auto maybeNode = node.getChild(CSTR_BACKGROUND))
        {
            component->mainMenuBackground = maybeNode->value_;
        }
        if (auto maybeStartStateNode = node.getChild(CSTR_START_STATE))
        {
            const auto& value = maybeStartStateNode->value_;

            if (auto maybeValue = magic_enum::enum_cast<PauseMenu::State>(value))
            {
                component->startState  = *maybeValue;
            }
        }
        return component;
    };
    DEBUG_LOG("registerReadFunctions");
    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}

void PauseMenuComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    node.addChild(CSTR_LOGO_FILE, pauseMenuImage.GetDataRelativeDir());
    node.addChild(CSTR_BACKGROUND, mainMenuBackground.GetDataRelativeDir());
    node.addChild(CSTR_START_STATE, magic_enum::enum_name(startState));
}

extern "C" void registerReadFunction()
{
    LOG_DEBUG << "PauseMenuComponent::registerReadFunctions";
    PauseMenuComponent::registerReadFunctions();
}
