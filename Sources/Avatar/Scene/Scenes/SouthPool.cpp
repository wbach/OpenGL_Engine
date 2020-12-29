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
#include <Logger/Log.h>
#include <Thread.hpp>

using namespace GameEngine;

namespace AvatarGame
{
class SettingViewer
{
public:
    SettingViewer(GuiElementFactory& factory, GuiManager& guiManager)
        : factory_{factory}
        , guiManager_{guiManager}
        , categoriesWindow_{nullptr}
        , paramsWindow_{nullptr}
    {
        auto categoriesWindow =
            factory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, vec2(-0.5, 0.f), vec2(0.2f, 1.f));
        categoriesWindow_ = categoriesWindow.get();
        auto logo         = factory_.CreateGuiTexture("Gui/1200px-Avatar_The_Last_Airbender_logo.svg.png");
        logo->SetScale({categoriesWindow->GetScale().x, categoriesWindow->GetScale().x});
        logo->SetPostion({0, 0.5f});
        categoriesWindow_->AddChild(std::move(logo));
        auto verticalLayout = factory_.CreateVerticalLayout();
        verticalLayout->SetPostion({0, -0.5});
        verticalLayout->SetScale(categoriesWindow->GetScale() * 0.5f);
        verticalLayout->SetAlgin(Layout::Algin::CENTER);

        auto categories = configurationExplorer_.getCatogiresList();
        for (auto& category : categories)
        {
            auto categoryText = factory_.CreateGuiText(category);
            auto categoryButton =
                factory_.CreateGuiButton([&, categoryName = category, categoryTextPtr = categoryText.get()](auto&) {
                    if (paramsWindow_)
                    {
                        guiManager_.Remove(*paramsWindow_);
                    }

                    auto paramsWindow =
                        factory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, vec2(0.25f, 0.f), vec2(0.5f, 0.5f));
                    paramsWindow_ = paramsWindow.get();

                    auto paramsVerticalLayout = factory_.CreateVerticalLayout();
                    paramsVerticalLayout->SetScale(paramsWindow->GetScale() * 0.99f);
                    paramsVerticalLayout->SetAlgin(Layout::Algin::LEFT);
                    paramsVerticalLayout->SetPostion({-.25, 0});

                    const auto& params = configurationExplorer_.getParamsFromCategory(categoryName);
                    for (auto& param : params)
                    {
                        auto horizontalLayout = factory_.CreateHorizontalLayout();
                        horizontalLayout->SetScale({paramsWindow->GetScale().x * 0.99f, 0.f});
                        horizontalLayout->SetAlgin(Layout::Algin::LEFT);

                        auto paramNameText = factory_.CreateGuiText(param.name);

                        auto paramText = factory_.CreateGuiText(" " + param.configurationParam.toString() + " ");

                        auto previousValueButton =
                            factory_.CreateGuiButton(" < ", [&param, paramTextPtr = paramText.get()](auto&) {
                                auto str = param.configurationParam.previous();
                                paramTextPtr->SetText(str);
                            });

                        auto nextValueButton =
                            factory_.CreateGuiButton(" > ", [&param, paramTextPtr = paramText.get()](auto&) {
                                auto str = param.configurationParam.next();
                                paramTextPtr->SetText(str);
                            });

                        auto apllyButton =
                            factory_.CreateGuiButton(" apply ", [this, &param, paramTextPtr = paramText.get()](auto&) {
                                configurationChanged_ = true;
                                param.configurationParam.apply();
                            });

                        horizontalLayout->AddChild(std::move(paramNameText));
                        horizontalLayout->AddChild(std::move(previousValueButton));
                        horizontalLayout->AddChild(std::move(paramText));
                        horizontalLayout->AddChild(std::move(nextValueButton));
                        horizontalLayout->AddChild(std::move(apllyButton));
                        paramsVerticalLayout->AddChild(std::move(horizontalLayout));
                    }

                    paramsWindow_->AddChild(std::move(paramsVerticalLayout));
                    guiManager_.Add(std::move(paramsWindow));
                });
            categoryButton->SetScale({0.2, 0.05});
            categoryButton->SetText(std::move(categoryText));
            verticalLayout->AddChild(std::move(categoryButton));
        }

        auto backButton = factory_.CreateGuiButton("Back", [&](auto&) {
            auto window = guiManager_.GetElement("PauseMenuMainWindow");
            if (window)
                window->Show();
            if (categoriesWindow_)
                categoriesWindow_->Hide();
            if (paramsWindow_)
                paramsWindow_->Hide();
            if (configurationChanged_)
                WriteConfigurationToFile(EngineConf);
        });
        backButton->SetScale({0.2, 0.05});
        verticalLayout->AddChild(std::move(backButton));
        categoriesWindow->AddChild(std::move(verticalLayout));
        guiManager_.Add(std::move(categoriesWindow));
    }

    ~SettingViewer()
    {
        guiManager_.Remove(*categoriesWindow_);
        if (paramsWindow_)
            guiManager_.Remove(*paramsWindow_);
    }

    bool configurationChanged_{false};
    GuiElementFactory& factory_;
    GuiManager& guiManager_;

    GuiWindowElement* categoriesWindow_;
    GuiWindowElement* paramsWindow_;
    GameEngine::ConfigurationExplorer configurationExplorer_;
};  // namespace AvatarGame

std::unique_ptr<SettingViewer> settingViewer_;

const std::string pauseMenuFile = "Scenes/PauseMenu/PauseMenu.xml";

SouthPool::SouthPool()
    : Scene("SouthPool")
{
}

SouthPool::~SouthPool()
{
    DEBUG_LOG(__FUNCTION__);
    settingViewer_.reset();
}

int SouthPool::Initialize()
{
    DEBUG_LOG("SouthPool::Initialize()");

    // resourceManager_->GetTextureLaoder().CreateHeightMap("Textures/Terrain/HeightMaps/World.png",
    //                                                     "Textures/Terrain/HeightMaps/output.terrain", vec3(1.f));

    GuiTheme guiTheme;
    guiTheme.font      = "GUI/herculanum.ttf";
    guiTheme.fontSize_ = 50;

    guiElementFactory_->SetTheme(guiTheme);
    inputManager_->SubscribeOnKeyDown(KeyCodes::F1, [&]() { addEngineEvent(EngineEvent::QUIT); });

    const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/SouthPool/SouthPool.xml");
    LoadFromFile(sceneFile);

    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->GetDebugRenderer().Enable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->GetDebugRenderer().Disable(); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() {
        auto window = guiManager_->GetElement("PauseMenuMainWindow");
        if (window)
        {
            if (window->IsShow())
            {
                window->Hide();
                camera.Unlock();
            }
            else
            {
                window->Show();
                camera.Lock();
            }
        }
        else
        {
            camera.Unlock();
        }

        if (settingViewer_)
            settingViewer_.reset();
    });

    // inputManager_->SubscribeOnKeyDown(KeyCodes::TAB, [&]() {
    //    auto state = *EngineConf.renderer.shadows.isEnabled;
    //    EngineConf.renderer.shadows.isEnabled.set(not state);
    //});

    // inputManager_->SubscribeOnKeyDown(KeyCodes::TAB, [&]() {
    //    auto state = *EngineConf.renderer.water.type;
    //    if (state == GameEngine::Params::WaterType::FULL)
    //        EngineConf.renderer.water.type.set(GameEngine::Params::WaterType::SIMPLE);
    //    else
    //        EngineConf.renderer.water.type.set(GameEngine::Params::WaterType::FULL);
    //});

    inputManager_->SubscribeOnKeyDown(KeyCodes::TAB, [&]() {
        auto& windowSizeParam = EngineConf.window.size;
        auto nextValue        = windowSizeParam.next();
        DEBUG_LOG("windowSizeParam nextValue : " + nextValue);
        windowSizeParam.apply();
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::ENTER, [&]() {
        auto& param = EngineConf.window.fullScreen;
        param.set(not*param);
    });

    guiManager_->RegisterAction("Settings()", [&](auto&) {
        settingViewer_.reset(new SettingViewer(*guiElementFactory_, *guiManager_));
        auto window = guiManager_->GetElement("PauseMenuMainWindow");
        if (window)
        {
            window->Hide();
        }
    });

    guiManager_->RegisterAction("BackToMainMenu()", [&](auto&) {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_ID, 0);
        addSceneEvent(sceneEvent);
    });

    guiManager_->RegisterAction("Resume()", [&](auto&) {
        auto window = guiManager_->GetElement("PauseMenuMainWindow");
        if (window)
        {
            if (window->IsShow())
            {
                window->Hide();
                camera.Unlock();
            }
            else
            {
                window->Show();
                camera.Lock();
            }
        }

        if (settingViewer_)
            settingViewer_.reset();
    });

    guiManager_->RegisterAction("ExitGame()", [&](auto&) { addEngineEvent(EngineEvent::QUIT); });

    guiElementFactory_->ReadGuiFile(EngineConf_GetFullDataPath(pauseMenuFile));

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
}  // namespace AvatarGame
