#include "EditorScene.h"

#include <GameEngine/Animations/AnimationUtils.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Menu/Menu.h>
#include <Input/InputManager.h>
#include <Types.h>
#include <Utils/Image/ImageUtils.h>
#include <Utils/Variant.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "Editor/Context.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"

using namespace GameEngine;

namespace Editor
{
std::vector<std::string> exportAnimationClips(
    const std::optional<Animation::Joint>& maybeRootJoint,
    const Components::Animator::AnimationInfoClips& animationClips, const GameEngine::File& file)
{
    std::cout << file.GetAbsoultePath() << std::endl;

    std::cout << "Animation clips : " << std::to_string(animationClips.size()) << std::endl;

    auto outputpath = std::filesystem::path(file.GetAbsoultePath()).parent_path().string() + "/output/" +
                      file.GetBaseName() + "_animationClips/";
    std::filesystem::create_directories(outputpath);

    std::vector<std::string> result;

    if (maybeRootJoint)
    {
        for (const auto& [name, info] : animationClips)
        {
            std::cout << "-- " << name << std::endl;
            std::string outputFile = Utils::GetAbsolutePath(outputpath) + "/" + name + ".xml";
            GameEngine::Animation::ExportAnimationClipToFile(outputFile, info.clip, *maybeRootJoint);
            result.push_back(outputFile);
        }
    }

    std::cout << file.GetBaseName() << " done." << std::endl;
    return result;
}

EditorScene::EditorScene()
    : GameEngine::Scene("EditorScene")
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

    // auto menu = guiElementFactory_->CreateMenu();
    // menu->AddRootElement("File");
    // menu->Add("File", "Open", [](GuiElement& button) {});
    // menu->Add("File", "Save", [](GuiElement& button) {});
    // menu->Add("File", "Exit", [](GuiElement& button) {});

    // menu->AddRootElement("Edit");
    // menu->Add("Edit", "Preferences", [](GuiElement& button) {});

    //// menu->AddRootElement("View");
    // menu->AddRootElement("Help");
    // menu->Add("Help", "About", [](GuiElement& button) {});
    // guiManager_->Add(std::move(menu));

    auto text = guiElementFactory_->CreateGuiText("Drag object to preview");
    text->SetScreenScale({0.1, 0.05});
    text->SetScreenPostion({0.05, 0.05});
    text->SetColor(vec4(0.05, 0.05, 0.05, 1.f));
    guiManager_->Add(std::move(text));

    auto actionsButtonsLayotPtr = guiElementFactory_->CreateVerticalLayout();
    auto actionsButtonsLayot    = actionsButtonsLayotPtr.get();
    actionsButtonsLayotPtr->SetScreenPostion({0.8625, 0.0});
    actionsButtonsLayotPtr->SetScreenScale({0.125, 1.f});
    guiManager_->Add(std::move(actionsButtonsLayotPtr));

    auto button = guiElementFactory_->CreateGuiButton("Convert mesh to png heightmap", [&](auto&) {
        if (not gameObject)
            return;
        uint32 heightmapResultuion = 512;
        float step                 = 1.f / static_cast<float>(heightmapResultuion);

        Utils::Image image;
        image.width  = heightmapResultuion;
        image.height = heightmapResultuion;
        image.setChannels(4);
        image.allocateImage<uint8>();

        std::vector<std::vector<float>> heights;
        heights.reserve(heightmapResultuion);
        std::optional<float> maxHeight;
        std::optional<float> minHeight;

        for (float y = -0.5f; y < 0.5f; y += step)
        {
            heights.push_back({});
            auto& row = heights.back();

            for (float x = -0.5f; x < 0.5f; x += step)
            {
                vec3 from(x, 5.f, y);
                vec3 to(x, -5.f, y);
                auto maybeHit = physicsApi_->RayTest(from, to);

                float height{0.f};
                maybeHit ? height = maybeHit->pointWorld.y : 0.f;

                if (not maxHeight or (*maxHeight) < height)
                {
                    maxHeight = height;
                }

                if (not minHeight or (*minHeight) > height)
                {
                    minHeight = height;
                }
                row.push_back(height);
            }
        }

        for (uint32 y = 0; y < heightmapResultuion; y++)
        {
            for (uint32 x = 0; x < heightmapResultuion; x++)
            {
                auto normalizedHeight = (heights[y][x] - (*minHeight)) / ((*maxHeight) - (*minHeight));
                Color color(normalizedHeight, normalizedHeight, normalizedHeight, 1.f);
                // uint8_t* array;
                // array = reinterpret_cast<uint8_t*>(&normalizedHeight);
                // Color color(array[0], array[1], array[2], array[3]);
                image.setPixel(vec2ui{y, x}, color);
            }
        }
        Utils::SaveImage(image, "heightmap.png");
    });
    button->SetLocalScale({1.f, 0.05f});
    actionsButtonsLayot->AddChild(std::move(button));

    auto avaiableAnimationsPtr = guiElementFactory_->CreateVerticalLayout();
    auto avaiableAnimations    = avaiableAnimationsPtr.get();
    avaiableAnimationsPtr->SetScreenPostion({0.0625, 0.5});
    avaiableAnimationsPtr->SetScreenScale({0.125, 1.f});
    guiManager_->Add(std::move(avaiableAnimationsPtr));

    graphicsApi_->GetWindowApi().SubscribeForEvent([&, animationLayout = avaiableAnimations](auto& event) {
        std::visit(
            visitor{
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
                    auto& component                    = newGameObject->AddComponent<Components::RendererComponent>();
                    auto& animator                     = newGameObject->AddComponent<Components::Animator>();
                    animator.startupAnimationClipName_ = "noname";
                    component.AddModel(dropFileEvent.filename);

                    newGameObject->AddComponent<Components::MeshShape>();
                    newGameObject->AddComponent<Components::Rigidbody>();

                    auto refreshClipsAction = [&, &clips = animator.getAnimationClips(),
                                               inputFilename = dropFileEvent.filename](const auto&) {
                        for (auto id : clipsButtonsIds_)
                        {
                            animationLayout->RemoveChild(id);
                        }
                        for (const auto& [name, _] : clips)
                        {
                            auto button2 = guiElementFactory_->CreateGuiButton(name, [&, animName = name](const auto&) {
                                animator.ChangeAnimation(animName, Components::Animator::AnimationChangeType::smooth);
                            });
                            button2->SetScreenScale({1.f, 0.05f});
                            clipsButtonsIds_.push_back(button2->GetId());
                            animationLayout->AddChild(std::move(button2));
                        }

                        auto exportButton = guiElementFactory_->CreateGuiButton(
                            "ExportAllAnimationsToXml", [&, filename = inputFilename](const auto&) {
                                auto model = component.GetModelWrapper().Get();
                                if (model)
                                {
                                    auto rootJoint = model->getRootJoint();
                                    exportAnimationClips(rootJoint, clips, filename);
                                }
                            });
                        exportButton->SetScreenScale({1.f, 0.05f});
                        clipsButtonsIds_.push_back(exportButton->GetId());
                        animationLayout->AddChild(std::move(exportButton));
                    };

                    auto button = guiElementFactory_->CreateGuiButton("Refresh clips", refreshClipsAction);

                    button->SetScreenScale({1.f, 0.05f});
                    animationLayout->AddChild(std::move(button));

                    auto files =
                        Utils::FindFilesWithExtension(GameEngine::File(dropFileEvent.filename).GetParentDir(), ".xml");
                    if (not files.empty())
                    {
                        for (const auto& file : files)
                        {
                            DEBUG_LOG("Found animation file in subfolders add clip : " + file);
                            if (auto animationName = GameEngine::Animation::IsAnimationClip(file))
                            {
                                animator.AddAnimationClip(GameEngine::File(file));
                                // auto button = guiElementFactory_->CreateGuiButton(
                                //    *animationName, [&, animName = *animationName](const auto&) {
                                //        DEBUG_LOG(animName);
                                //        animator.ChangeAnimation(
                                //            animName, Components::Animator::AnimationChangeType::smooth);
                                //    });
                                // button->SetScreenScale({1.f, 0.05f});
                                // clipsButtonsIds_.push_back(button->GetId());
                                // animationLayout->AddChild(std::move(button));
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
int EditorScene::Update(float dt)
{
    if (gameObject and inputManager_->GetKey(KeyCodes::LMOUSE))
    {
        auto currentRotation  = gameObject->GetWorldTransform().GetRotation().value_;
        auto v                = inputManager_->CalcualteMouseMove();
        const float sensitive = dt;
        if (v.x != 0)
        {
            gameObject->SetWorldRotation(currentRotation * glm::angleAxis(sensitive * float(v.x), vec3(0.f, 1.f, 0.f)));
        }
        // if (v.y != 0)
        //{
        //    gameObject->SetWorldRotation(currentRotation * glm::angleAxis(sensitive * float(v.x), vec3(0.f,
        //    0.f, 1.f)));
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
