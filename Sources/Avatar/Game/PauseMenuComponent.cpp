#include "PauseMenuComponent.h"

#include <GameEngine/Components/ComponentsReadFunctions.h>
#include <Logger/Log.h>

#include <GameEngine/Scene/Scene.hpp>

#include "PauseMenuTheme.h"

using namespace GameEngine::Components;
using namespace GameEngine;

namespace
{
const std::string COMPONENT_STR{"PauseMenu"};
}

PauseMenuComponent::PauseMenuComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(PauseMenuComponent).hash_code(), componentContext, gameObject)
    , startState{PauseMenu::State::MainMenu}
{
}

void PauseMenuComponent::CleanUp()
{
}

void PauseMenuComponent::ReqisterFunctions()
{
    DEBUG_LOG("Hello from PauseMenuComponent");
    RegisterFunction(FunctionType::Awake,
                     [this]()
                     {
                         DEBUG_LOG("Create pause menu");
                         auto& guiElementFactory = componentContext_.guiElementFactory_;
                         guiElementFactory.SetTheme(getGuiTheme());

                         const auto& avaiableScenes = componentContext_.sceneManager_.GetAvaiableScenes();

                         pauseMenu =
                             std::make_unique<PauseMenu>(startState, componentContext_.scene_, guiElementFactory, avaiableScenes);

                         if (startState == PauseMenu::State::MainMenu)
                         {
                             auto background = guiElementFactory.CreateGuiTexture("GUI/204473.jpg");
                             background->SetZPosition(100.f);
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
        if (auto maybeStartStateNode = node.getChild("startState"))
        {
            const auto& value = maybeStartStateNode->value_;
            if (value == "MainMenu")
            {
                component->startState = PauseMenu::State::MainMenu;
            }
            else if (value == "PauseMenu")
            {
                component->startState = PauseMenu::State::PauseMenu;
            }
            else if (value == "SettingsView")
            {
                component->startState = PauseMenu::State::SettingsView;
            }
            else if (value == "SceneLoader")
            {
                component->startState = PauseMenu::State::SceneLoader;
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
    auto& startStateNode = node.addChild("startState");
    switch (startState)
    {
        case PauseMenu::State::MainMenu:
            startStateNode.value_ = "MainMenu";
            break;
        case PauseMenu::State::PauseMenu:
            startStateNode.value_ = "PauseMenu";
            break;
        case PauseMenu::State::SettingsView:
            startStateNode.value_ = "SettingsView";
            break;
        case PauseMenu::State::SceneLoader:
            startStateNode.value_ = "SceneLoader";
            break;
    };
}

extern "C" void registerReadFunction()
{
    LOG_DEBUG << "PauseMenuComponent::registerReadFunctions";
    PauseMenuComponent::registerReadFunctions();
}
