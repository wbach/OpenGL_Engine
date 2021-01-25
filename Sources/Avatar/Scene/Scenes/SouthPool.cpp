#include "SouthPool.h"

#include <GLM/GLMUtils.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/GuiRenderer.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <GameEngine/Renderers/RenderersManager.h>
#include <GameEngine/Resources/ResourceManager.h>
#include <GameEngine/Resources/Textures/HeightMap.h>
#include <GameEngine/Renderers/GUI/Layer/DefaultLayers.h>
#include <Logger/Log.h>

#include <Thread.hpp>

using namespace GameEngine;

namespace AvatarGame
{
class Menu
{
public:
    Menu(Scene& scene, GuiElementFactory& factory, GuiManager& guiManager)
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
    ~Menu()
    {
        if (mainWindow_)
            guiManager_.Remove(*mainWindow_);
        for (auto& pair : settingWindows_)
            guiManager_.Remove(*pair.second);
    }

    void show()
    {
        scene_.GetCamera().Lock();
        enablePauseMenuState();
        onePramaterNeedRestart_ = false;

        auto hudLayer = guiManager_.GetLayer(DefaultGuiLayers::hud);
        if (hudLayer)
        {
            hudLayer->Hide();
        }
    }
    void hide()
    {
        scene_.GetCamera().Unlock();
        hideSettingWindows();
        mainWindow_->Hide();
        onePramaterNeedRestart_ = false;

        auto hudLayer = guiManager_.GetLayer(DefaultGuiLayers::hud);
        if (hudLayer)
        {
            hudLayer->Show();
        }
    }
    bool isShow() const
    {
        return mainWindow_->IsShow();
    }

private:
    void enablePauseMenuState()
    {
        guiManager_.AddTask([this]() {
            hideSettingWindows();
            settingsLayout_->Hide();
            pauseMenuLayout_->Show();
            ;
            mainWindow_->Show();
        });
    }
    void enableSettingsViewState()
    {
        guiManager_.AddTask([this]() {
            hideSettingWindows();
            pauseMenuLayout_->Hide();
            settingsLayout_->Show();
            mainWindow_->Show();
        });
    }

    void init()
    {
        auto mainWindow = factory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, vec2(0.25f, 0.5f), vec2(0.2f, 1.f));
        mainWindow->Hide();

        mainWindow_ = mainWindow.get();
        auto logo   = factory_.CreateGuiTexture("GUI/1200px-Avatar_The_Last_Airbender_logo.svg.png");
        logo->SetLocalScale({1.f, 0.15f});
        logo->SetLocalPostion({0.5f, 0.75f});
        mainWindow_->AddChild(std::move(logo));

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

    void setMainWindowVerticalLayoutTransform(VerticalLayout& layout)
    {
        layout.SetLocalPostion({0.5f, 0.25});
        layout.SetLocalScale({1.f, 0.5f});
        layout.SetAlgin(Layout::Algin::CENTER);
    }

    std::unique_ptr<VerticalLayout> createPauseMenuLayout()
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

    std::unique_ptr<VerticalLayout> createSettingsLayout()
    {
        auto verticalLayout = factory_.CreateVerticalLayout();

        auto categories = configurationExplorer_.getCatogiresList();
        for (auto& category : categories)
        {
            auto paramsWindow =
                factory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, vec2(0.675, 0.5f), vec2(0.64f, 1.0f));
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

        auto backButton = factory_.CreateGuiButton("Back", [&](auto&) { enablePauseMenuState(); });

        backButton->SetLocalScale(menuButtonSize_);
        verticalLayout->AddChild(std::move(backButton));

        return verticalLayout;
    }

    void fillSettingsParamWindow(VerticalLayout& paramsVerticalLayout, const std::string& categoryName)
    {
        const auto& params = configurationExplorer_.getParamsFromCategory(categoryName);

        for (auto& param : params)
        {
            auto horizontalLayout = factory_.CreateHorizontalLayout();
            horizontalLayout->SetAlgin(Layout::Algin::CENTER);
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

            auto nextValueButton =
                factory_.CreateGuiButton(" > ", [this, &param, paramTextPtr = paramText.get()](auto&) {
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
        horizontalLayout->SetAlgin(Layout::Algin::CENTER);
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

    void hideSettingWindows()
    {
        for (auto& pair : settingWindows_)
        {
            pair.second->Hide();
        }
    }

    void showSettingWindow(const std::string& name)
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

    void createMessageBox(const std::string& messageText)
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
        auto messageBoxWindow = factory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, {0.5f, 0.5f}, {0.5f, 0.25f});
        messageBox_           = messageBoxWindow.get();
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

private:
    bool onePramaterNeedRestart_{false};

    Scene& scene_;
    GuiElementFactory& factory_;
    GuiManager& guiManager_;

    GuiWindowElement* messageBox_;
    GuiWindowElement* mainWindow_;
    VerticalLayout* settingsLayout_;
    VerticalLayout* pauseMenuLayout_;
    VerticalLayout* mainMenuLayout_;
    GameEngine::ConfigurationExplorer configurationExplorer_;

    std::unordered_map<std::string, GuiWindowElement*> settingWindows_;

    std::mutex eventMutex_;
    std::vector<std::function<void()>> events_;

    const vec2& menuButtonSize_;
};  // namespace AvatarGame

std::unique_ptr<Menu> menu_;

const std::string pauseMenuFile = "Scenes/PauseMenu/PauseMenu.xml";

SouthPool::SouthPool()
    : Scene("SouthPool")
{
}

SouthPool::~SouthPool()
{
    DEBUG_LOG(__FUNCTION__);
    menu_.reset();
}

int SouthPool::Initialize()
{
    DEBUG_LOG("SouthPool::Initialize()");

    prepareMenu();

    // resourceManager_->GetTextureLaoder().CreateHeightMap("Textures/Terrain/HeightMaps/World.png",
    //                                                     "Textures/Terrain/HeightMaps/output.terrain", vec3(1.f));

    inputManager_->SubscribeOnKeyDown(KeyCodes::F1, [&]() { addEngineEvent(EngineEvent::QUIT); });

    const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/SouthPool/SouthPool.xml");
    LoadFromFile(sceneFile);

    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->GetDebugRenderer().Enable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->GetDebugRenderer().Disable(); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { menu_->isShow() ? menu_->hide() : menu_->show(); });

    DEBUG_LOG("SouthPool::Initialized");
    return 0;
}

void SouthPool::PostInitialize()
{
}

int SouthPool::Update(float)
{
    return 0;
}
void SouthPool::prepareMenu()
{
    GuiTheme guiTheme;
    guiTheme.font      = "GUI/herculanum.ttf";
    guiTheme.fontSize_ = 50;
    guiElementFactory_->SetTheme(guiTheme);

    guiManager_->RegisterAction("BackToMainMenu()", [&](auto&) {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_ID, 0);
        addSceneEvent(sceneEvent);
    });
    guiManager_->RegisterAction("ExitGame()", [&](auto&) { addEngineEvent(EngineEvent::QUIT); });

    menu_ = std::make_unique<Menu>(*this, *guiElementFactory_, *guiManager_);
}
}  // namespace AvatarGame
