#include "PauseMenu.h"

#include <GLM/GLMUtils.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/GuiRenderer.h>
#include <GameEngine/Renderers/GUI/Layer/DefaultLayers.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <GameEngine/Renderers/RenderersManager.h>
#include <GameEngine/Resources/ResourceManager.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <Logger/Log.h>

#include <Thread.hpp>

#include "GameEngine/Scene/Scene.hpp"

namespace AvatarGame
{
PauseMenu::PauseMenu(GameEngine::Scene& scene, GameEngine::GuiElementFactory& factory,
                     GameEngine::GuiManager& guiManager)
    : scene_{scene}
    , factory_{factory}
    , guiManager_{guiManager}
    , messageBox_{nullptr}
    , mainWindow_{nullptr}
    , settingsLayout_{nullptr}
    , pauseMenuLayout_{nullptr}
    , mainMenuLayout_{nullptr}
    , menuButtonSize_{1.f, 0.075f}
{
    init();
}

PauseMenu::~PauseMenu()
{
    if (mainWindow_)
        guiManager_.Remove(*mainWindow_);
    for (auto& pair : settingWindows_)
        guiManager_.Remove(*pair.second);
}

void PauseMenu::show(State state)
{
    scene_.GetCamera().Lock();
    startState_ = state;
    enableState(state);

    onePramaterNeedRestart_ = false;
    auto hudLayer           = guiManager_.GetLayer(GameEngine::DefaultGuiLayers::hud);
    if (hudLayer)
    {
        hudLayer->Hide();
    }
}

void PauseMenu::hide()
{
    scene_.GetCamera().Unlock();
    hideSettingWindows();
    mainWindow_->Hide();
    onePramaterNeedRestart_ = false;

    auto hudLayer = guiManager_.GetLayer(GameEngine::DefaultGuiLayers::hud);
    if (hudLayer)
    {
        hudLayer->Show();
    }
}

bool PauseMenu::isShow() const
{
    return mainWindow_->IsShow();
}

void PauseMenu::enableState(State state)
{
    switch (state)
    {
        case State::MainMenu:
            enableMainMenuState();
            break;
        case State::PauseMenu:
            enablePauseMenuState();
            break;
        case State::SettingsView:
            enableSettingsViewState();
            break;
    }
}

void PauseMenu::backToStartState()
{
    enableState(startState_);
}

void PauseMenu::enableMainMenuState()
{
    guiManager_.AddTask([this]() {
        hideSettingWindows();
        mainMenuLayout_->Show();
        settingsLayout_->Hide();
        pauseMenuLayout_->Hide();
        mainWindow_->Show();
    });
}

void PauseMenu::enablePauseMenuState()
{
    guiManager_.AddTask([this]() {
        hideSettingWindows();
        mainMenuLayout_->Hide();
        settingsLayout_->Hide();
        pauseMenuLayout_->Show();
        mainWindow_->Show();
    });
}

void PauseMenu::enableSettingsViewState()
{
    guiManager_.AddTask([this]() {
        hideSettingWindows();
        mainMenuLayout_->Hide();
        pauseMenuLayout_->Hide();
        settingsLayout_->Show();
        mainWindow_->Show();
    });
}

void PauseMenu::init()
{
    auto mainWindow =
        factory_.CreateGuiWindow(GameEngine::GuiWindowStyle::BACKGROUND_ONLY, vec2(0.25f, 0.5f), vec2(0.2f, 1.f));
    mainWindow->Hide();

    mainWindow_ = mainWindow.get();
    auto logo   = factory_.CreateGuiTexture("GUI/1200px-Avatar_The_Last_Airbender_logo.svg.png");
    logo->SetLocalScale({1.f, 0.15f});
    logo->SetLocalPostion({0.5f, 0.75f});
    mainWindow_->AddChild(std::move(logo));

    auto mainMenuLayout = createMainMenuLayout();
    mainMenuLayout_     = mainMenuLayout.get();
    setMainWindowVerticalLayoutTransform(*mainMenuLayout);
    mainWindow_->AddChild(std::move(mainMenuLayout));

    auto pauseLayout = createPauseMenuLayout();
    pauseMenuLayout_ = pauseLayout.get();
    setMainWindowVerticalLayoutTransform(*pauseLayout);
    mainWindow_->AddChild(std::move(pauseLayout));

    auto settingsLayout = createSettingsLayout();
    settingsLayout_     = settingsLayout.get();
    setMainWindowVerticalLayoutTransform(*settingsLayout);
    mainWindow_->AddChild(std::move(settingsLayout));

    guiManager_.Add(std::move(mainWindow));
}

void PauseMenu::setMainWindowVerticalLayoutTransform(GameEngine::VerticalLayout& layout)
{
    layout.SetLocalPostion({0.5f, 0.25});
    layout.SetLocalScale({1.f, 0.5f});
    layout.SetAlgin(GameEngine::Layout::Algin::CENTER);
}

std::unique_ptr<GameEngine::VerticalLayout> PauseMenu::createMainMenuLayout()
{
    auto verticalLayout = factory_.CreateVerticalLayout();
    {
        auto guiButton = factory_.CreateGuiButton("New game", guiManager_.GetActionFunction("StartGame()"));
        guiButton->SetLocalScale(menuButtonSize_);
        verticalLayout->AddChild(std::move(guiButton));
    }
    {
        auto guiButton =
            factory_.CreateGuiButton("Load game", [this](auto&) { createMessageBox("Not implemented yet"); });
        guiButton->SetLocalScale(menuButtonSize_);
        verticalLayout->AddChild(std::move(guiButton));
    }
    {
        auto guiButton = factory_.CreateGuiButton("Settings", [this](auto&) { enableSettingsViewState(); });

        guiButton->SetLocalScale(menuButtonSize_);
        verticalLayout->AddChild(std::move(guiButton));
    }
    {
        auto guiButton = factory_.CreateGuiButton("Exit game", guiManager_.GetActionFunction("ExitGame()"));
        guiButton->SetLocalScale(menuButtonSize_);
        verticalLayout->AddChild(std::move(guiButton));
    }
    return verticalLayout;
}

std::unique_ptr<GameEngine::VerticalLayout> PauseMenu::createPauseMenuLayout()
{
    auto verticalLayout = factory_.CreateVerticalLayout();
    {
        auto guiButton = factory_.CreateGuiButton("Resume", [this](auto&) { hide(); });
        guiButton->SetLocalScale(menuButtonSize_);
        verticalLayout->AddChild(std::move(guiButton));
    }
    {
        auto guiButton = factory_.CreateGuiButton("Settings", [this](auto&) { enableSettingsViewState(); });

        guiButton->SetLocalScale(menuButtonSize_);
        verticalLayout->AddChild(std::move(guiButton));
    }
    {
        auto guiButton =
            factory_.CreateGuiButton("Back to main menu", guiManager_.GetActionFunction("BackToMainMenu()"));
        guiButton->SetLocalScale(menuButtonSize_);
        verticalLayout->AddChild(std::move(guiButton));
    }
    {
        auto guiButton = factory_.CreateGuiButton("Exit game", guiManager_.GetActionFunction("ExitGame()"));
        guiButton->SetLocalScale(menuButtonSize_);
        verticalLayout->AddChild(std::move(guiButton));
    }

    return verticalLayout;
}

std::unique_ptr<GameEngine::VerticalLayout> PauseMenu::createSettingsLayout()
{
    auto verticalLayout = factory_.CreateVerticalLayout();

    auto categories = configurationExplorer_.getCatogiresList();
    for (auto& category : categories)
    {
        auto paramsWindow =
            factory_.CreateGuiWindow(GameEngine::GuiWindowStyle::BACKGROUND_ONLY, vec2(0.675, 0.5f), vec2(0.64f, 1.0f));
        paramsWindow->Hide();
        settingWindows_.insert({category, paramsWindow.get()});
        auto paramsVerticalLayout = factory_.CreateVerticalLayout();
        fillSettingsParamWindow(*paramsVerticalLayout, category);
        paramsWindow->AddChild(std::move(paramsVerticalLayout));
        guiManager_.Add(std::move(paramsWindow));

        auto categoryText = factory_.CreateGuiText(category);
        auto categoryButton =
            factory_.CreateGuiButton([&, categoryName = category, categoryTextPtr = categoryText.get()](auto&) {
                showSettingWindow(categoryName);
            });

        categoryButton->SetLocalScale(menuButtonSize_);
        categoryButton->SetText(std::move(categoryText));
        verticalLayout->AddChild(std::move(categoryButton));
    }

    auto backButton = factory_.CreateGuiButton("Back", [&](auto&) { backToStartState(); });

    backButton->SetLocalScale(menuButtonSize_);
    verticalLayout->AddChild(std::move(backButton));

    return verticalLayout;
}

void PauseMenu::fillSettingsParamWindow(GameEngine::VerticalLayout& paramsVerticalLayout,
                                        const std::string& categoryName)
{
    const auto& params = configurationExplorer_.getParamsFromCategory(categoryName);

    for (auto& param : params)
    {
        auto horizontalLayout = factory_.CreateHorizontalLayout();
        horizontalLayout->SetAlgin(GameEngine::Layout::Algin::CENTER);
        horizontalLayout->SetLocalScale({1.f, 0.0375f});

        auto paramNameText = factory_.CreateGuiText(param.name);
        paramNameText->SetLocalScale({0.5f, 1.f});

        auto paramText = factory_.CreateGuiText(" " + param.configurationParam.toString() + " ");
        paramText->SetLocalScale({0.2f, 1.f});

        auto previousValueButton =
            factory_.CreateGuiButton(" < ", [this, &param, paramTextPtr = paramText.get()](auto&) {
                auto str = param.configurationParam.previous();
                paramTextPtr->SetText(str);

                if (param.restartRequierd)
                {
                    onePramaterNeedRestart_ = true;
                }
            });
        previousValueButton->SetLocalScale({0.05f, 1.f});

        auto nextValueButton = factory_.CreateGuiButton(" > ", [this, &param, paramTextPtr = paramText.get()](auto&) {
            auto str = param.configurationParam.next();
            paramTextPtr->SetText(str);

            if (param.restartRequierd)
            {
                onePramaterNeedRestart_ = true;
            }
        });
        nextValueButton->SetLocalScale({0.05f, 1.f});

        std::string applyStr = param.restartRequierd ? " apply. " : " apply ";
        auto apllyButton     = factory_.CreateGuiButton(applyStr, [this, &param](auto&) {
            param.configurationParam.apply();
            if (param.restartRequierd)
            {
                createMessageBox("Change will be visible after game restart");
            }
            WriteConfigurationToFile(EngineConf);
        });
        apllyButton->SetLocalScale({0.1f, 1.f});
        horizontalLayout->AddChild(std::move(paramNameText));
        horizontalLayout->AddChild(std::move(previousValueButton));
        horizontalLayout->AddChild(std::move(paramText));
        horizontalLayout->AddChild(std::move(nextValueButton));
        horizontalLayout->AddChild(std::move(apllyButton));
        paramsVerticalLayout.AddChild(std::move(horizontalLayout));
    }

    auto horizontalLayout = factory_.CreateHorizontalLayout();
    horizontalLayout->SetAlgin(GameEngine::Layout::Algin::CENTER);
    horizontalLayout->SetLocalScale({1.f, 0.0375f});
    auto apllyButton = factory_.CreateGuiButton(" apply all ", [this, categoryName](auto&) {
        const auto& params = configurationExplorer_.getParamsFromCategory(categoryName);
        for (auto& param : params)
        {
            param.configurationParam.apply();
        }

        if (onePramaterNeedRestart_)
        {
            createMessageBox("One from changed param need restart game");
            onePramaterNeedRestart_ = false;
        }
        WriteConfigurationToFile(EngineConf);
    });

    // to do : button without horizontal layout position issue

    horizontalLayout->SetLocalScale({1.f, 0.0375f});
    horizontalLayout->AddChild(std::move(apllyButton));
    paramsVerticalLayout.AddChild(std::move(horizontalLayout));
}

void PauseMenu::hideSettingWindows()
{
    for (auto& pair : settingWindows_)
    {
        pair.second->Hide();
    }
}

void PauseMenu::showSettingWindow(const std::string& name)
{
    for (auto& pair : settingWindows_)
    {
        if (pair.first == name)
        {
            pair.second->Show();
        }
        else
        {
            pair.second->Hide();
        }
    }
}

void PauseMenu::createMessageBox(const std::string& messageText)
{
    if (messageBox_)
    {
        guiManager_.Remove(*messageBox_);
    }
    auto orginalTheme               = factory_.GetTheme();
    auto modifiedTheme              = orginalTheme;
    modifiedTheme.backgroundTexture = "GUI/darkGrayButtonActive.png";
    modifiedTheme.buttonTexture     = "GUI/darkGrayButtonActive.png";
    factory_.SetTheme(modifiedTheme);

    vec2 buttonSize{0.3f, 0.1f};
    auto messageBoxWindow =
        factory_.CreateGuiWindow(GameEngine::GuiWindowStyle::BACKGROUND_ONLY, {0.5f, 0.5f}, {0.5f, 0.25f});
    messageBox_ = messageBoxWindow.get();
    messageBoxWindow->SetZPosition(-100.f);

    auto text = factory_.CreateGuiText(messageText);
    text->SetLocalScale({0.9f, 0.5f});
    text->SetLocalPostion({0.5f, 0.5f});
    messageBoxWindow->AddChild(std::move(text));

    auto okbutton = factory_.CreateGuiButton("ok", [this](auto&) {
        if (messageBox_)
        {
            guiManager_.AddRemoveTask(messageBox_);
            messageBox_ = nullptr;
        }
    });

    okbutton->SetLocalScale(buttonSize);
    okbutton->SetLocalPostion({0.5f, 0.2f});
    messageBoxWindow->AddChild(std::move(okbutton));

    guiManager_.Add(std::move(messageBoxWindow));

    factory_.SetTheme(orginalTheme);
}

}  // namespace AvatarGame
