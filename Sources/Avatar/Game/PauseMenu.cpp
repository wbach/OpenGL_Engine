#include "PauseMenu.h"

#include <GLM/GLMUtils.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include <GameEngine/Engine/EngineEvent.h>
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
PauseMenu::PauseMenu(State startState, GameEngine::Scene& scene, GameEngine::GuiElementFactory& factory,
                     GameEngine::GuiManager& guiManager)
    : PauseMenu(startState, scene, factory, guiManager, {})
{
}

PauseMenu::PauseMenu(State startState, GameEngine::Scene& scene, GameEngine::GuiElementFactory& factory,
                     GameEngine::GuiManager& guiManager, const std::vector<std::string>& avaiableSscenes)
    : startState_{startState}
    , scene_{scene}
    , factory_{factory}
    , guiManager_{guiManager}
    , messageBox_{nullptr}
    , mainWindow_{nullptr}
    , currentView_{nullptr}
    , menuButtonSize_{1.f, 0.075f}
    , avaiableScenes_{avaiableSscenes}
    , keysSubscriptionsManager{*scene_.getInputManager()}
{
    init();

    escapeSub = scene_.getInputManager()->SubscribeOnKeyDown(KeyCodes::ESCAPE,
                                                             [this]()
                                                             {
                                                                 if (not isShow())
                                                                 {
                                                                     show();
                                                                 }
                                                             });
}

void PauseMenu::subscribeKeys()
{
    keysSubscriptionsManager.AddSubscribtion(scene_.getInputManager()->SubscribeOnKeyDown(
        KeyCodes::DARROW,
        [&]()
        {
            guiManager_.AddTask(
                [&]()
                {
                    if (currentView_)
                    {
                        currentView_->second.getActiveButtons().updateSelectedRow(1);
                    }
                });
        }));
    keysSubscriptionsManager.AddSubscribtion(scene_.getInputManager()->SubscribeOnKeyDown(
        KeyCodes::UARROW,
        [&]()
        {
            guiManager_.AddTask(
                [&]()
                {
                    if (currentView_)
                    {
                        currentView_->second.getActiveButtons().updateSelectedRow(-1);
                    }
                });
        }));
    keysSubscriptionsManager.AddSubscribtion(scene_.getInputManager()->SubscribeOnKeyDown(
        KeyCodes::RARROW,
        [&]()
        {
            guiManager_.AddTask(
                [&]()
                {
                    if (currentView_)
                    {
                        currentView_->second.getActiveButtons().updateSelectedColumn(1);
                    }
                });
        }));
    keysSubscriptionsManager.AddSubscribtion(scene_.getInputManager()->SubscribeOnKeyDown(
        KeyCodes::LARROW,
        [&]()
        {
            guiManager_.AddTask(
                [&]()
                {
                    if (currentView_)
                    {
                        currentView_->second.getActiveButtons().updateSelectedColumn(-1);
                    }
                });
        }));

    keysSubscriptionsManager.AddSubscribtion(scene_.getInputManager()->SubscribeOnKeyDown(
        KeyCodes::ENTER,
        [&]()
        {
            guiManager_.AddTask(
                [&]()
                {
                    if (currentView_)
                    {
                        currentView_->second.getActiveButtons().exectuteAction();
                    }
                });
        }));
    keysSubscriptionsManager.AddSubscribtion(scene_.getInputManager()->SubscribeOnKeyDown(
        KeyCodes::ESCAPE,
        [&]()
        {
            guiManager_.AddTask(
                [&]()
                {
                    if (currentView_)
                    {
                        if (currentView_->second.isFocusToChild())
                        {
                            currentView_->second.getActiveButtons().toneDownCurrent();
                            currentView_->second.resetFocus();
                            currentView_->second.getActiveButtons().highLightCurrent();
                            return;
                        }

                        if (currentView_->first == State::MainMenu)
                        {
                            scene_.addEngineEvent(GameEngine::EngineEvent::ASK_QUIT);
                            return;
                        }
                        if (currentView_->first == State::PauseMenu)
                        {
                            hide();
                            return;
                        }
                        backToStartState();
                    }
                });
        }));
}
PauseMenu::~PauseMenu()
{
    for (auto& [subid, configParam] : paramChangeSubs_)
    {
        DEBUG_LOG("unsubscribe " + std::to_string(subid));
        configParam->unsubscribe(subid);
    }

    if (mainWindow_)
        guiManager_.Remove(*mainWindow_);

    for (auto& [_, view] : views_)
    {
        for (auto& [_, child] : view.children)
            guiManager_.Remove(*child.window);
    }
}

void PauseMenu::show()
{
    scene_.getInputManager()->SetReleativeMouseMode(false);
    scene_.getInputManager()->ShowCursor(true);

    scene_.GetCamera().Lock();
    enableState(startState_);

    onePramaterNeedRestart_ = false;
    auto hudLayer           = guiManager_.GetLayer(GameEngine::DefaultGuiLayers::hud);
    if (hudLayer)
    {
        hudLayer->Hide();
    }
    subscribeKeys();
}

void PauseMenu::hide()
{
    keysSubscriptionsManager.UnsubscribeKeys();
    scene_.getInputManager()->SetReleativeMouseMode(true);
    scene_.getInputManager()->ShowCursor(false);

    scene_.GetCamera().Unlock();

    for (auto& [_, view] : views_)
    {
        for (auto& [_, child] : view.children)
        {
            child.window->Hide();
        }
    }
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
        case State::SceneLoader:
            enableSceneLoaderViewState();
            break;
    }
}

void PauseMenu::backToStartState()
{
    if (currentView_)
        currentView_->second.getActiveButtons().toneDownCurrent();
    enableState(startState_);
}

void PauseMenu::enableMainMenuState()
{
    guiManager_.AddTask([this]() { setViewAsActive(State::MainMenu); });
}

void PauseMenu::enablePauseMenuState()
{
    guiManager_.AddTask([this]() { setViewAsActive(State::PauseMenu); });
}

void PauseMenu::enableSettingsViewState()
{
    guiManager_.AddTask([this]() { setViewAsActive(State::SettingsView); });
}

void PauseMenu::enableSceneLoaderViewState()
{
    guiManager_.AddTask([this]() { setViewAsActive(State::SceneLoader); });
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

    createMainMenuLayout();
    createPauseMenuLayout();
    createSettingsView();
    createSceneLoaderLayout();

    guiManager_.Add(std::move(mainWindow));
}

void PauseMenu::setMainWindowVerticalLayoutTransform(GameEngine::VerticalLayout& layout)
{
    layout.SetLocalPostion({0.5f, 0.25});
    layout.SetLocalScale({1.f, 0.5f});
    layout.SetAlgin(GameEngine::Layout::Algin::CENTER);
}

void PauseMenu::createMainMenuLayout()
{
    auto& view          = views_[State::MainMenu];
    auto verticalLayout = factory_.CreateVerticalLayout();
    view.layout         = verticalLayout.get();

    createButtonForLayout(view, "New game", guiManager_.GetActionFunction("StartGame()"));
    createButtonForLayout(view, "Load game", [this](auto&) { createMessageBox("Not implemented yet"); });
    createButtonForLayout(view, "Start specyfic scene", [this](auto&) { enableSceneLoaderViewState(); });
    createButtonForLayout(view, "Settings", [this](auto&) { enableSettingsViewState(); });
    createButtonForLayout(view, "Exit game", guiManager_.GetActionFunction("ExitGame()"));

    verticalLayout->Hide();
    setMainWindowVerticalLayoutTransform(*verticalLayout);
    mainWindow_->AddChild(std::move(verticalLayout));
}

void PauseMenu::createPauseMenuLayout()
{
    auto& view          = views_[State::PauseMenu];
    auto verticalLayout = factory_.CreateVerticalLayout();
    view.layout         = verticalLayout.get();

    createButtonForLayout(view, "Resume", [this](auto&) { hide(); });
    createButtonForLayout(view, "Load game", [this](auto&) { createMessageBox("Not implemented yet"); });
    createButtonForLayout(view, "Save game", [this](auto&) { createMessageBox("Not implemented yet"); });
    createButtonForLayout(view, "Settings", [this](auto&) { enableSettingsViewState(); });
    createButtonForLayout(view, "Back to main menu", guiManager_.GetActionFunction("BackToMainMenu()"));
    createButtonForLayout(view, "Exit game", guiManager_.GetActionFunction("ExitGame()"));

    verticalLayout->Hide();
    setMainWindowVerticalLayoutTransform(*verticalLayout);
    mainWindow_->AddChild(std::move(verticalLayout));
}

void PauseMenu::createSettingsView()
{
    auto& view = views_[State::SettingsView];

    auto verticalLayout = factory_.CreateVerticalLayout();
    view.layout         = verticalLayout.get();

    auto categories = configurationExplorer_.getCatogiresList();
    for (auto& category : categories)
    {
        auto paramsWindow =
            factory_.CreateGuiWindow(GameEngine::GuiWindowStyle::BACKGROUND_ONLY, vec2(0.675, 0.5f), vec2(0.64f, 1.0f));
        paramsWindow->Hide();
        auto& subWidnow  = view.children[category];
        subWidnow.window = paramsWindow.get();

        fillSettingsParamWindow(subWidnow, category);
        guiManager_.Add(std::move(paramsWindow));

        auto categoryText = factory_.CreateGuiText(category);
        auto categoryButton =
            factory_.CreateGuiButton([&, categoryName = category](auto&) { showSettingWindow(view, categoryName); });
        view.getActiveButtons().array.push_back({categoryButton.get()});

        categoryButton->SetLocalScale(menuButtonSize_);
        categoryButton->SetText(std::move(categoryText));
        verticalLayout->AddChild(std::move(categoryButton));
    }

    auto backButton = factory_.CreateGuiButton("Back", [&](auto&) { backToStartState(); });
    view.getActiveButtons().array.push_back({backButton.get()});
    backButton->SetLocalScale(menuButtonSize_);
    verticalLayout->AddChild(std::move(backButton));

    verticalLayout->Hide();
    setMainWindowVerticalLayoutTransform(*verticalLayout);

    mainWindow_->AddChild(std::move(verticalLayout));
}

void PauseMenu::createSceneLoaderLayout()
{
    auto& view = views_[State::SceneLoader];

    auto verticalLayout = factory_.CreateVerticalLayout();
    for (const auto& sceneName : avaiableScenes_)
    {
        auto guiButton = factory_.CreateGuiButton(sceneName,
                                                  [this, sceneName](auto&)
                                                  {
                                                      GameEngine::SceneEvent sceneEvent(
                                                          GameEngine::SceneEventType::LOAD_SCENE_BY_NAME, sceneName);
                                                      scene_.SendEvent(sceneEvent);
                                                  });
        guiButton->SetLocalScale(menuButtonSize_);
        view.getActiveButtons().array.push_back({guiButton.get()});
        verticalLayout->AddChild(std::move(guiButton));
    }
    {
        auto guiButton = factory_.CreateGuiButton("Back to main menu", [this](const auto&) { enableMainMenuState(); });
        guiButton->SetLocalScale(menuButtonSize_);
        view.getActiveButtons().array.push_back({guiButton.get()});
        verticalLayout->AddChild(std::move(guiButton));
    }

    verticalLayout->Hide();
    setMainWindowVerticalLayoutTransform(*verticalLayout);
    view.layout = verticalLayout.get();
    mainWindow_->AddChild(std::move(verticalLayout));
}

void PauseMenu::fillSettingsParamWindow(View::Child& view, const std::string& categoryName)
{
    auto paramsVerticalLayout = factory_.CreateVerticalLayout();
    // view.buttons.array.resize(3);

    const auto& params = configurationExplorer_.getParamsFromCategory(categoryName);

    for (auto& param : params)
    {
        auto& row = view.buttons.createRow();

        auto horizontalLayout = factory_.CreateHorizontalLayout();
        horizontalLayout->SetAlgin(GameEngine::Layout::Algin::CENTER);
        horizontalLayout->SetLocalScale({1.f, 0.0375f});

        auto paramNameText = factory_.CreateGuiText(param.name);
        paramNameText->SetLocalScale({0.5f, 1.f});

        auto paramText = factory_.CreateGuiText(" " + param.configurationParam.toString() + " ");
        paramText->SetLocalScale({0.2f, 1.f});

        auto id = param.configurationParam.subscribeForChange(
            [guiText = paramText.get(), &param]() { guiText->SetText(param.configurationParam.toString()); });
        paramChangeSubs_.push_back({id, &param.configurationParam});

        auto previousValueButton = factory_.CreateGuiButton(
            " < ",
            [this, &param, paramTextPtr = paramText.get()](auto&)
            {
                auto str = param.configurationParam.previous();
                paramTextPtr->SetText(str);

                if (param.restartRequierd == GameEngine::ConfigurationExplorer::ApplyPolicy::RestartRequired)
                {
                    onePramaterNeedRestart_ = true;
                }
            });
        previousValueButton->SetLocalScale({0.05f, 1.f});
        row.push_back(previousValueButton.get());

        auto nextValueButton = factory_.CreateGuiButton(
            " > ",
            [this, &param, paramTextPtr = paramText.get()](auto&)
            {
                auto str = param.configurationParam.next();
                paramTextPtr->SetText(str);

                if (param.restartRequierd == GameEngine::ConfigurationExplorer::ApplyPolicy::RestartRequired)
                {
                    onePramaterNeedRestart_ = true;
                }
            });
        nextValueButton->SetLocalScale({0.05f, 1.f});
        row.push_back(nextValueButton.get());

        auto apllyButton = factory_.CreateGuiButton(
            "apply",
            [this, &param](auto&)
            {
                param.configurationParam.apply();
                if (param.restartRequierd == GameEngine::ConfigurationExplorer::ApplyPolicy::RestartRequired)
                {
                    createMessageBox("Change will be visible after game restart");
                }
                WriteConfigurationToFile(EngineConf);
            });
        row.push_back(apllyButton.get());

        if (param.restartRequierd == GameEngine::ConfigurationExplorer::ApplyPolicy::RestartRequired)
        {
            apllyButton->GetText()->SetColor(Color(1.f, 1.f, 0.f, 1.f).value);
        }

        apllyButton->SetLocalScale({0.1f, 1.f});
        horizontalLayout->AddChild(std::move(paramNameText));
        horizontalLayout->AddChild(std::move(previousValueButton));
        horizontalLayout->AddChild(std::move(paramText));
        horizontalLayout->AddChild(std::move(nextValueButton));
        horizontalLayout->AddChild(std::move(apllyButton));
        paramsVerticalLayout->AddChild(std::move(horizontalLayout));
    }

    auto horizontalLayout = factory_.CreateHorizontalLayout();
    horizontalLayout->SetAlgin(GameEngine::Layout::Algin::CENTER);
    horizontalLayout->SetLocalScale({1.f, 0.0375f});
    auto apllyButton = factory_.CreateGuiButton(" apply all ",
                                                [this, categoryName](auto&)
                                                {
                                                    const auto& params =
                                                        configurationExplorer_.getParamsFromCategory(categoryName);
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
    view.buttons.array.push_back({apllyButton.get()});
    // to do : button without horizontal layout position issue

    horizontalLayout->SetLocalScale({1.f, 0.0375f});
    horizontalLayout->AddChild(std::move(apllyButton));
    paramsVerticalLayout->AddChild(std::move(horizontalLayout));
    view.window->AddChild(std::move(paramsVerticalLayout));
}

void PauseMenu::showSettingWindow(View& view, const std::string& name)
{
    for (auto& [childName, child] : view.children)
    {
        if (childName == name)
        {
            child.window->Show();
            child.buttons.highLightCurrent();
            view.getActiveButtons().toneDownCurrent();
            view.setFocusToChildrenArray(child);
        }
        else
        {
            child.window->Hide();
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

    auto okbutton = factory_.CreateGuiButton("ok",
                                             [this](auto&)
                                             {
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

void PauseMenu::setViewAsActive(State state)
{
    auto iter = views_.find(state);
    if (iter == views_.end())
        return;

    View* view = &iter->second;
    if (not view->layout)
        return;

    if (currentView_)
    {
        if (currentView_->second.layout)
            currentView_->second.layout->Hide();

        for (auto& [_, child] : currentView_->second.children)
        {
            child.window->Hide();
        }
    }
    view->layout->Show();
    view->getActiveButtons().highLightCurrent();

    currentView_ = &(*iter);
    mainWindow_->Show();
}

void PauseMenu::View::Buttons::updateSelectedRow(int v)
{
    if (not array.empty())
    {
        if (isInRange())
            get().ToneDown();

        selectedRow += v;

        if (selectedRow < 0)
            selectedRow = static_cast<int>(array.size() - 1);
        if (selectedRow >= static_cast<int>(array.size()))
            selectedRow = 0;

        if (selectedColumn >= static_cast<int>(array[selectedRow].size()))
            selectedColumn = static_cast<int>(array[selectedRow].size() - 1);

        highLightCurrent();
    }
}

void PauseMenu::View::Buttons::updateSelectedColumn(int v)
{
    if (not array.empty())
    {
        if (isInRange())
            get().ToneDown();

        selectedColumn += v;

        if (selectedColumn < 0)
            selectedColumn = static_cast<int>(array[selectedRow].size() - 1);
        if (selectedColumn >= static_cast<int>(array[selectedRow].size()))
            selectedColumn = 0;

        highLightCurrent();
    }
}

void PauseMenu::View::Buttons::exectuteAction()
{
    if (isInRange())
        get().ExecuteAction();
}

bool PauseMenu::View::Buttons::isInRange() const
{
    return (selectedRow >= 0 and selectedRow < static_cast<int>(array.size())) and
           (selectedColumn >= 0 and selectedColumn < static_cast<int>(array[selectedRow].size()));
}

GameEngine::GuiButtonElement& PauseMenu::View::Buttons::get()
{
    return *array[selectedRow][selectedColumn];
}

void PauseMenu::createButtonForLayout(View& view, const std::string& text,
                                      std::function<void(GameEngine::GuiElement&)> action)
{
    auto guiButton = factory_.CreateGuiButton(text, action);
    guiButton->SetLocalScale(menuButtonSize_);
    view.getActiveButtons().array.push_back({guiButton.get()});
    view.layout->AddChild(std::move(guiButton));
}

PauseMenu::State PauseMenu::getCurrentState() const
{
    return currentView_ ? currentView_->first : startState_;
}

std::vector<GameEngine::GuiButtonElement*>& PauseMenu::View::Buttons::createRow()
{
    array.push_back({});
    return array.back();
}

void PauseMenu::View::Buttons::highLightCurrent()
{
    for (auto& column : array)
        for (auto& b : column)
            b->ToneDown();

    if (isInRange())
        get().Highlight();
}

void PauseMenu::View::Buttons::toneDownCurrent()
{
    if (isInRange())
        get().ToneDown();
}

}  // namespace AvatarGame
