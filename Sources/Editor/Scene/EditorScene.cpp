#include "EditorScene.h"

#include <GameEngine/Animations/AnimationUtils.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <Input/InputManager.h>
#include <Types.h>
#include <Utils/Variant.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "Editor/Context.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"

using namespace GameEngine;

namespace Editor
{
EditorScene::EditorScene(Context& context)
    : GameEngine::Scene("EditorScene")
    , context_(context)
    , gameObject(nullptr)
{
}
EditorScene::~EditorScene()
{
}

int EditorScene::Initialize()
{
    camera.SetPosition(vec3(2, 2, 2));
    camera.LookAt(vec3(0, 0.5, 0));
    camera.UpdateMatrix();

    RunNetworkEditorInterface();
    renderersManager_->GetDebugRenderer().Enable();

    // const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/TestSene.xml");
    // LoadFromFile(sceneFile);
    auto text = guiElementFactory_->CreateGuiText("Drag object to preview");
    text->SetScreenScale({0.1, 0.05});
    text->SetScreenPostion({0.05, 0.05});
    text->SetColor(vec4(0.05, 0.05, 0.05, 1.f));
    guiManager_->Add(std::move(text));

    auto avaiableAnimationsPtr = guiElementFactory_->CreateVerticalLayout();
    auto avaiableAnimations    = avaiableAnimationsPtr.get();
    avaiableAnimationsPtr->SetScreenPostion({0.0625, 0.5});
    avaiableAnimationsPtr->SetScreenScale({0.125, 1.f});
    guiManager_->Add(std::move(avaiableAnimationsPtr));

    graphicsApi_->GetWindowApi().SubscribeForEvent([&, animationLayout = avaiableAnimations](auto& event) {
        std::visit(visitor{
                       [&](const GraphicsApi::DropFileEvent& dropFileEvent) {
                           if (dropFileEvent.filename.empty())
                               return;

                           if (gameObject)
                           {
                               animationLayout->RemoveAll();
                               RemoveGameObject(*gameObject);
                           }

                           auto newGameObject = CreateGameObject();
                           DEBUG_LOG(dropFileEvent.filename);
                           auto& component = newGameObject->AddComponent<Components::RendererComponent>();
                           auto& animator  = newGameObject->AddComponent<Components::Animator>();
                           animator.startupAnimationClipName_ = "noname";
                           component.AddModel(dropFileEvent.filename);

                           auto files = Utils::FindFilesWithExtension(
                               GameEngine::File(dropFileEvent.filename).GetParentDir(), ".xml");

                           auto button = guiElementFactory_->CreateGuiButton(
                               "Refresh clips", [&, &clips = animator.animationClips_](const auto&) {
                                   for (const auto& [name, _] : clips)
                                   {
                                       auto button2 =
                                           guiElementFactory_->CreateGuiButton(name, [&, animName = name](const auto&) {
                                               animator.ChangeAnimation(
                                                   animName, Components::Animator::AnimationChangeType::smooth);
                                           });
                                       button2->SetScreenScale({1.f, 0.05f});
                                       animationLayout->AddChild(std::move(button2));
                                   }
                               });
                           button->SetScreenScale({1.f, 0.05f});
                           animationLayout->AddChild(std::move(button));

                           if (not files.empty())
                           {
                               for (const auto& file : files)
                               {
                                   DEBUG_LOG(file);
                                   if (auto animationName = GameEngine::Animation::IsAnimationClip(file))
                                   {
                                       animator.AddAnimationClip(GameEngine::File(file));
                                       auto button = guiElementFactory_->CreateGuiButton(
                                           *animationName, [&, animName = *animationName](const auto&) {
                                               DEBUG_LOG(animName);
                                               animator.ChangeAnimation(
                                                   animName, Components::Animator::AnimationChangeType::smooth);
                                           });
                                       button->SetScreenScale({1.f, 0.05f});
                                       animationLayout->AddChild(std::move(button));
                                   }
                               }

                               auto animationName = GameEngine::Animation::IsAnimationClip(files.front());
                               if (animationName)
                               {
                                   animator.startupAnimationClipName_ = *animationName;
                               }
                           }

                           gameObject = newGameObject.get();
                           AddGameObject(std::move(newGameObject));
                       },
                       [](const GraphicsApi::QuitEvent&) {},

                   },
                   event);
    });

    KeySubscribtions();

    return 0;
}
void EditorScene::PostInitialize()
{
}
int EditorScene::Update(float)
{
    if (gameObject and inputManager_->GetKey(KeyCodes::LMOUSE))
    {
        auto currentRotation = gameObject->GetWorldTransform().GetRotation().value_;
        auto v               = inputManager_->CalcualteMouseMove();
        const float sensitive = 0.01f;
        if (v.x != 0)
        {
            gameObject->SetWorldRotation(currentRotation * glm::angleAxis(sensitive * float(v.x), vec3(0.f, 1.f, 0.f)));
        }
        //if (v.y != 0)
        //{
        //    gameObject->SetWorldRotation(currentRotation * glm::angleAxis(sensitive * float(v.x), vec3(0.f, 0.f, 1.f)));
        //}
    }
    return 0;
}

void EditorScene::AddGameObjects(GameObject* go, int lvl)
{
    if (lvl == 0)
    {
        return;
    }

    for (uint32 i = 0; i < 4; ++i)
    {
        auto newGo = CreateGameObject(go->GetName() + "_" + std::to_string(i));
        newGo->AddComponent<GameEngine::Components::BoxShape>();
        newGo->AddComponent<GameEngine::Components::MeshShape>();

        AddGameObjects(newGo.get(), lvl - 1);
        go->AddChild(std::move(newGo));
    }
}

void EditorScene::KeySubscribtions()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->GetDebugRenderer().Enable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->GetDebugRenderer().Disable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::ASK_QUIT); });
    inputManager_->SubscribeOnKeyDown(
        KeyCodes::L, [renderersManager = this->renderersManager_]() { renderersManager->SwapLineFaceRender(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::J, [&]() {
        for (auto& go : GetGameObjects())
        {
            go->GetTransform().IncrasePosition(vec3(0.001f));
        }
    });
}
}  // namespace Editor
