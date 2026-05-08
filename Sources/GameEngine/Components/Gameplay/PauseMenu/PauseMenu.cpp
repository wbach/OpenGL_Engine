#include "PauseMenu.h"

#include <GLM/GLMUtils.h>
#include <GameEngine/Components/ComponentContext.h>
#include <GameEngine/Components/Input/PlayerInputController.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include <GameEngine/Engine/EngineEvent.h>
#include <GameEngine/Renderers/GUI/Button/Button.h>
#include <GameEngine/Renderers/GUI/Layer/DefaultLayers.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Renderer.h>
#include <GameEngine/Renderers/GUI/Sprite/Sprite.h>
#include <GameEngine/Renderers/GUI/Window/Window.h>
#include <GameEngine/Renderers/RenderersManager.h>
#include <GameEngine/Resources/ResourceManager.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <Logger/Log.h>

#include <GameEngine/Scene/Scene.hpp>
#include <Thread.hpp>

#include "GameEngine/Renderers/GUI/HorizontalAlign.h"
#include "GameEngine/Renderers/GUI/Transform.h"
#include "GameEngine/Renderers/GUI/VerticalAlign.h"

namespace GameEngine
{
namespace Components
{
PauseMenu::PauseMenu(File& logoFile, State startState,ComponentContext& context,
                     const vec2& menuButtonSize)
    : PauseMenu(logoFile, startState, context, menuButtonSize, {})
{
}

PauseMenu::PauseMenu(File& logoFile, State startState,ComponentContext& context,
                     const vec2& menuButtonSize, const std::unordered_map<std::string, uint32>& avaiableScenes)
    : context(context)
    , logoFile{logoFile}
    , startState_{startState}
    , scene_{context.scene_}
    , factory_{context.guiElementFactory_}
    , guiManager_{context.guiManager_}
    , messageBox_{nullptr}
    , mainWindow_{nullptr}
    , currentView_{nullptr}
    , menuButtonSize_{menuButtonSize}
    , avaiableScenes_{avaiableScenes}
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

PauseMenu::~PauseMenu()
{
    keysSubscriptionsManager.UnsubscribeKeys();

    for (auto& [subid, configParam] : paramChangeSubs_)
    {
        LOG_DEBUG << "unsubscribe " << subid;
        configParam->unsubscribe(subid);
    }

    if (mainWindow_)
        guiManager_.remove(*mainWindow_);

    for (auto& [_, view] : views_)
    {
        for (auto& [_, child] : view.children)
            guiManager_.remove(*child.window);
    }

    LOG_DEBUG << "PauseMenu destructor done";
}
void PauseMenu::subscribeKeys()
{
    keysSubscriptionsManager.AddSubscribtion(
        scene_.getInputManager()->SubscribeOnKeyDown(KeyCodes::DARROW,
                                                     [&]()
                                                     {
                                                         guiManager_.addTask(
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
            guiManager_.addTask(
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
            guiManager_.addTask(
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
            guiManager_.addTask(
                [&]()
                {
                    if (currentView_)
                    {
                        currentView_->second.getActiveButtons().updateSelectedColumn(-1);
                    }
                });
        }));

    keysSubscriptionsManager.AddSubscribtion(
        scene_.getInputManager()->SubscribeOnKeyDown(KeyCodes::ENTER,
                                                     [&]()
                                                     {
                                                         guiManager_.addTask(
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
            LOG_DEBUG << "AddTask";
            guiManager_.addTask(
                [&]()
                {
                    LOG_DEBUG << "Task execute";
                    if (currentView_)
                    {
                        if (currentView_->second.isFocusToChild())
                        {
                            LOG_DEBUG << "isFocusToChild";
                            currentView_->second.getActiveButtons().toneDownCurrent();
                            currentView_->second.resetFocus();
                            currentView_->second.getActiveButtons().highLightCurrent();
                            return;
                        }

                        if (currentView_->first == State::MainMenu)
                        {
                            LOG_DEBUG << "MainMenu";
                            scene_.addEngineEvent(QuitEvent{QuitEvent::ASK_QUIT});
                            return;
                        }
                        if (currentView_->first == State::PauseMenu)
                        {
                            LOG_DEBUG << "hide";
                            hide();
                            return;
                        }
                        LOG_DEBUG << "backToStartState";
                        backToStartState();
                    }
                });
            LOG_DEBUG << "ESCAPE done";
        }));
}

void PauseMenu::show()
{
    auto playerComponents = context.componentController_.GetAllComponentsOfType<Components::PlayerInputController>();
    LOG_DEBUG << "PlayerInputController size : " << playerComponents.size();
    for (auto& playerComponent : playerComponents)
    {
        // if (playerComponent->IsInitated())
        {
            playerComponentToRestore.insert(playerComponent);
            playerComponent->CleanUp();
        }
    }
    // scene_.getInputManager()->StashSubscribers();
    scene_.getInputManager()->SetReleativeMouseMode(false);
    scene_.getInputManager()->ShowCursor(true);

    auto mainCamera = scene_.GetCameraManager().GetMainCamera();
    if (mainCamera)
    {
        mainCamera->Lock();

        cameraRotationBeforeShow = mainCamera->GetRotation();
        cameraPositionBeforShow  = mainCamera->GetPosition();
    }

    enableState(startState_);

    onePramaterNeedRestart_ = false;
    auto hudLayer           = guiManager_.getLayer(DefaultGuiLayers::hud);
    if (hudLayer)
    {
        hudLayer->activate(false);
    }
    subscribeKeys();
}

void PauseMenu::hide()
{
    auto playerComponents =
        context.componentController_.GetAllActiveComponentsOfType<Components::PlayerInputController>();
    for (auto& playerComponent : playerComponents)
    {
        auto iter = playerComponentToRestore.find(playerComponent);

        if (iter != playerComponentToRestore.end())
        {
            playerComponent->Init();
        }
        else
        {
            LOG_DEBUG << "Player input controller not exist anymore";
        }
    }
    playerComponentToRestore.clear();

    keysSubscriptionsManager.UnsubscribeKeys();
    // scene_.getInputManager()->StashPopSubscribers();
    scene_.getInputManager()->SetReleativeMouseMode(true);
    scene_.getInputManager()->ShowCursor(false);

    auto mainCamera = scene_.GetCameraManager().GetMainCamera();
    if (not mainCamera)
    {
        return;
    }
    mainCamera->Unlock();
    mainCamera->SetRotation(cameraRotationBeforeShow);
    mainCamera->SetPosition(cameraPositionBeforShow);

    for (auto& [_, view] : views_)
    {
        for (auto& [_, child] : view.children)
        {
            child.window->activate(false);
        }
    }
    mainWindow_->activate(false);
    onePramaterNeedRestart_ = false;

    auto hudLayer = guiManager_.getLayer(DefaultGuiLayers::hud);
    if (hudLayer)
    {
        hudLayer->activate(true);
    }
}

bool PauseMenu::isShow() const
{
    return mainWindow_->isActive();
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
    guiManager_.addTask([this]() { setViewAsActive(State::MainMenu); });
}

void PauseMenu::enablePauseMenuState()
{
    guiManager_.addTask([this]() { setViewAsActive(State::PauseMenu); });
}

void PauseMenu::enableSettingsViewState()
{
    guiManager_.addTask([this]() { setViewAsActive(State::SettingsView); });
}

void PauseMenu::enableSceneLoaderViewState()
{
    guiManager_.addTask([this]() { setViewAsActive(State::SceneLoader); });
}

void PauseMenu::init()
{
    auto mainWindow = factory_.createWindow(GUI::WindowStyle::BACKGROUND_ONLY);
    mainWindow->setTransform(GUI::Transform{.position = vec2(0.25f, 0.5f), .scale = vec2(0.2f, 1.f)});
    mainWindow->activate(false);

    mainWindow_ = mainWindow.get();
    auto logo   = factory_.createSprite(logoFile.GetAbsolutePath().string());
    logo->setLocalScale({1.f, 0.15f});
    logo->setLocalPosition({0.5f, 0.75f});
    mainWindow_->addChild(std::move(logo));

    createMainMenuLayout();
    createPauseMenuLayout();
    createSettingsView();
    createSceneLoaderLayout();

    guiManager_.add(std::move(mainWindow));
}

void PauseMenu::setMainWindowVerticalLayoutTransform(GUI::VerticalLayout& layout)
{
    layout.setLocalPosition({0.5f, 0.25});
    layout.setLocalScale({1.f, 0.5f});
    layout.setAlign(GUI::HorizontalAlign::CENTER);
}

void PauseMenu::createMainMenuLayout()
{
    auto& view          = views_[State::MainMenu];
    auto verticalLayout = factory_.createVerticalLayout();
    verticalLayout->setAlign(GUI::VerticalAlign::TOP);
    view.layout = verticalLayout.get();

    createButtonForLayout(
        view, "New game",
        [this]() {
            scene_.addEngineEvent(ChangeSceneEvent{ChangeSceneEvent::Type::LOAD_SCENE_BY_ID, 1});
        });
    createButtonForLayout(view, "Load game", [this]() { createMessageBox("Not implemented yet"); });
    createButtonForLayout(view, "Start specyfic scene", [this]() { enableSceneLoaderViewState(); });
    createButtonForLayout(view, "Settings", [this]() { enableSettingsViewState(); });
    createButtonForLayout(view, "Exit game",
                          [this]() { scene_.addEngineEvent(QuitEvent{QuitEvent::ASK_QUIT}); });

    verticalLayout->activate(false);
    setMainWindowVerticalLayoutTransform(*verticalLayout);
    mainWindow_->addChild(std::move(verticalLayout));
}

void PauseMenu::createPauseMenuLayout()
{
    auto& view          = views_[State::PauseMenu];
    auto verticalLayout = factory_.createVerticalLayout();
    verticalLayout->setAlign(GUI::VerticalAlign::TOP);
    view.layout = verticalLayout.get();

    createButtonForLayout(view, "Resume", [this]() { hide(); });
    createButtonForLayout(view, "Load game", [this]() { createMessageBox("Not implemented yet"); });
    createButtonForLayout(view, "Save game", [this]() { createMessageBox("Not implemented yet"); });
    createButtonForLayout(view, "Settings", [this]() { enableSettingsViewState(); });
    createButtonForLayout(
        view, "Back to main menu",
        [this]() {
            scene_.addEngineEvent(ChangeSceneEvent{ChangeSceneEvent::Type::LOAD_SCENE_BY_ID, 0});
        });
    createButtonForLayout(view, "Exit game",
                          [this]() { scene_.addEngineEvent(QuitEvent{QuitEvent::ASK_QUIT}); });

    verticalLayout->activate(false);
    setMainWindowVerticalLayoutTransform(*verticalLayout);
    mainWindow_->addChild(std::move(verticalLayout));
}

void PauseMenu::createSettingsView()
{
    auto& view = views_[State::SettingsView];

    auto verticalLayout = factory_.createVerticalLayout();
    verticalLayout->setAlign(GUI::VerticalAlign::TOP);
    view.layout = verticalLayout.get();

    auto categories = configurationExplorer_.getCatogiresList();
    for (auto& category : categories)
    {
        auto paramsWindow = factory_.createWindow(GUI::WindowStyle::BACKGROUND_ONLY);
        paramsWindow->setTransform(GUI::Transform{.position = vec2(0.675, 0.5f), .scale = vec2(0.64f, 1.0f)});
        paramsWindow->activate(false);
        auto& subWidnow  = view.children[category];
        subWidnow.window = paramsWindow.get();

        fillSettingsParamWindow(subWidnow, category);
        guiManager_.add(std::move(paramsWindow));

        auto categoryText   = factory_.createText(category);
        auto categoryButton = factory_.createButton();
        categoryButton->setOnClick([&, categoryName = category]() { showSettingWindow(view, categoryName); });
        view.getActiveButtons().array.push_back({categoryButton.get()});
        categoryButton->setLocalScale(menuButtonSize_);
        categoryButton->setText(std::move(categoryText));
        verticalLayout->addChild(std::move(categoryButton));
    }

    auto backButton = factory_.createButton("Back");
    backButton->setOnClick([&]() { backToStartState(); });
    view.getActiveButtons().array.push_back({backButton.get()});
    backButton->setLocalScale(menuButtonSize_);
    verticalLayout->addChild(std::move(backButton));

    verticalLayout->activate(false);
    setMainWindowVerticalLayoutTransform(*verticalLayout);

    mainWindow_->addChild(std::move(verticalLayout));
}

void PauseMenu::createSceneLoaderLayout()
{
    auto& view = views_[State::SceneLoader];

    auto verticalLayout = factory_.createVerticalLayout();
    verticalLayout->setAlign(GUI::VerticalAlign::TOP);
    for (const auto& [sceneName, _] : avaiableScenes_)
    {
        auto guiButton = factory_.createButton(sceneName);
        guiButton->setOnClick(
            [this, name = sceneName]() {
                scene_.addEngineEvent(ChangeSceneEvent{ChangeSceneEvent::Type::LOAD_SCENE_BY_NAME, name});
            });
        guiButton->setLocalScale(menuButtonSize_);
        view.getActiveButtons().array.push_back({guiButton.get()});
        verticalLayout->addChild(std::move(guiButton));
    }
    {
        auto guiButton = factory_.createButton("Back to main menu");
        guiButton->setOnClick([this]() { enableMainMenuState(); });
        guiButton->setLocalScale(menuButtonSize_);
        view.getActiveButtons().array.push_back({guiButton.get()});
        verticalLayout->addChild(std::move(guiButton));
    }

    verticalLayout->activate(false);
    setMainWindowVerticalLayoutTransform(*verticalLayout);
    view.layout = verticalLayout.get();
    mainWindow_->addChild(std::move(verticalLayout));
}

void PauseMenu::fillSettingsParamWindow(View::Child& view, const std::string& categoryName)
{
    auto paramsVerticalLayout = factory_.createVerticalLayout();
    paramsVerticalLayout->setAlign(GUI::VerticalAlign::TOP);
    // view.buttons.array.resize(3);

    const auto& params = configurationExplorer_.getParamsFromCategory(categoryName);

    for (auto& param : params)
    {
        auto& row = view.buttons.createRow();

        auto horizontalLayout = factory_.createHorizontalLayout();
        horizontalLayout->setAlign(GUI::HorizontalAlign::CENTER);
        horizontalLayout->setLocalScale({1.f, menuButtonSize_.y / 2.f});

        auto paramNameText = factory_.createText(param.name);
        paramNameText->setLocalScale({0.5f, 1.f});

        auto paramText = factory_.createText(" " + param.configurationParam.get().toString() + " ");
        paramText->setLocalScale({0.2f, 1.f});

        auto id = param.configurationParam.get().subscribeForChange(
            [guiText = paramText.get(), &param]() { guiText->setText(param.configurationParam.get().toString()); });
        paramChangeSubs_.push_back({id, &param.configurationParam.get()});

        auto previousValueButton = factory_.createButton(" < ");
        previousValueButton->setOnClick(
            [this, &param, paramTextPtr = paramText.get()]()
            {
                auto str = param.configurationParam.get().previous();
                paramTextPtr->setText(str);

                if (param.restartRequierd == ConfigurationExplorer::ApplyPolicy::RestartRequired)
                {
                    onePramaterNeedRestart_ = true;
                }
            });
        previousValueButton->setLocalScale({0.05f, 1.f});
        row.push_back(previousValueButton.get());

        auto nextValueButton = factory_.createButton(" > ");
        nextValueButton->setOnClick(
            [this, &param, paramTextPtr = paramText.get()]()
            {
                auto str = param.configurationParam.get().next();
                paramTextPtr->setText(str);

                if (param.restartRequierd == ConfigurationExplorer::ApplyPolicy::RestartRequired)
                {
                    onePramaterNeedRestart_ = true;
                }
            });
        nextValueButton->setLocalScale({0.05f, 1.f});
        row.push_back(nextValueButton.get());

        auto apllyButton = factory_.createButton("apply");
        apllyButton->setOnClick(
            [this, &param]()
            {
                param.configurationParam.get().apply();
                if (param.restartRequierd == ConfigurationExplorer::ApplyPolicy::RestartRequired)
                {
                    createMessageBox("Change will be visible after game restart");
                }
                WriteConfigurationToFile(EngineConf);
            });
        row.push_back(apllyButton.get());

        if (param.restartRequierd == ConfigurationExplorer::ApplyPolicy::RestartRequired)
        {
            apllyButton->getTextElement()->setColor(Color(1.f, 1.f, 0.f, 1.f).value);
        }

        apllyButton->setLocalScale({0.1f, 1.f});
        horizontalLayout->addChild(std::move(paramNameText));
        horizontalLayout->addChild(std::move(previousValueButton));
        horizontalLayout->addChild(std::move(paramText));
        horizontalLayout->addChild(std::move(nextValueButton));
        horizontalLayout->addChild(std::move(apllyButton));
        paramsVerticalLayout->addChild(std::move(horizontalLayout));
    }

    auto horizontalLayout = factory_.createHorizontalLayout();
    horizontalLayout->setAlign(GUI::HorizontalAlign::CENTER);
    horizontalLayout->setLocalScale({1.f, menuButtonSize_.y / 2.f});
    auto apllyButton = factory_.createButton(" apply all ");
    apllyButton->setLocalScale({0.2f, 1.f});
    apllyButton->setOnClick(
        [this, categoryName]()
        {
            const auto& params = configurationExplorer_.getParamsFromCategory(categoryName);
            for (auto& param : params)
            {
                param.configurationParam.get().apply();
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
    horizontalLayout->addChild(std::move(apllyButton));
    paramsVerticalLayout->addChild(std::move(horizontalLayout));
    view.window->addChild(std::move(paramsVerticalLayout));
}

void PauseMenu::showSettingWindow(View& view, const std::string& name)
{
    for (auto& [childName, child] : view.children)
    {
        if (childName == name)
        {
            child.window->activate(true);
            child.buttons.highLightCurrent();
            view.getActiveButtons().toneDownCurrent();
            view.setFocusToChildrenArray(child);
        }
        else
        {
            child.window->activate(false);
        }
    }
}

void PauseMenu::createMessageBox(const std::string& messageText)
{
    if (messageBox_)
    {
        guiManager_.remove(*messageBox_);
    }
    auto orginalTheme  = factory_.getTheme();
    auto modifiedTheme = orginalTheme;
    // modifiedTheme.backgroundTexture = "GUI/darkGrayButtonActive.png";
    // modifiedTheme.buttonTexture     = "GUI/darkGrayButtonActive.png";
    factory_.setTheme(modifiedTheme);

    vec2 buttonSize{0.3f, 0.1f};
    auto messageBoxWindow = factory_.createWindow(GUI::WindowStyle::BACKGROUND_ONLY);
    messageBoxWindow->setTransform(
        GUI::Transform{.position = {0.5f, 0.5f}, .scale = {0.5f, 0.25f}, .zValue = -100.f});
    messageBox_ = messageBoxWindow.get();

    auto text = factory_.createText(messageText);
    text->setLocalScale({0.9f, 0.5f});
    text->setLocalPosition({0.5f, 0.5f});
    messageBoxWindow->addChild(std::move(text));

    auto okbutton = factory_.createButton("ok");
    okbutton->setOnClick(
        [this]()
        {
            if (messageBox_)
            {
                guiManager_.addTask(
                    [&]()
                    {
                        guiManager_.remove(*messageBox_);
                        messageBox_ = nullptr;
                    });
            }
        });

    okbutton->setLocalScale(buttonSize);
    okbutton->setLocalPosition({0.5f, 0.2f});
    messageBoxWindow->addChild(std::move(okbutton));

    guiManager_.add(std::move(messageBoxWindow));

    factory_.setTheme(orginalTheme);
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
            currentView_->second.layout->activate(false);

        for (auto& [_, child] : currentView_->second.children)
        {
            child.window->activate(false);
        }
    }
    view->layout->activate(true);
    view->getActiveButtons().highLightCurrent();

    currentView_ = &(*iter);
    mainWindow_->activate(true);
}

void PauseMenu::View::Buttons::updateSelectedRow(int v)
{
    if (not array.empty())
    {
        if (isInRange())
            get().toneDown();

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
            get().toneDown();

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
        get().executeAction();
}

bool PauseMenu::View::Buttons::isInRange() const
{
    return (selectedRow >= 0 and selectedRow < static_cast<int>(array.size())) and
           (selectedColumn >= 0 and selectedColumn < static_cast<int>(array[selectedRow].size()));
}

GUI::Button& PauseMenu::View::Buttons::get()
{
    return *array[selectedRow][selectedColumn];
}

void PauseMenu::createButtonForLayout(View& view, const std::string& text, std::function<void()> action)
{
    auto guiButton = factory_.createButton(text);
    guiButton->setOnClick(action);
    guiButton->setLocalScale(menuButtonSize_);
    view.getActiveButtons().array.push_back({guiButton.get()});
    view.layout->addChild(std::move(guiButton));
}

PauseMenu::State PauseMenu::getCurrentState() const
{
    return currentView_ ? currentView_->first : startState_;
}

std::vector<GUI::Button*>& PauseMenu::View::Buttons::createRow()
{
    array.push_back({});
    return array.back();
}

void PauseMenu::View::Buttons::highLightCurrent()
{
    for (auto& column : array)
        for (auto& b : column)
            b->toneDown();

    if (isInRange())
        get().highlight();
}

void PauseMenu::View::Buttons::toneDownCurrent()
{
    if (isInRange())
        get().toneDown();
}
}  // namespace Components
}  // namespace GameEngine
