#include "NewScene.h"

#include <GameEngine/Components/Camera/CameraComponent.h>
#include <GameEngine/Components/Lights/DirectionalLightComponent.h>
#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>
#include <GameEngine/Resources/Models/Primitive.h>
#include <GameEngine/Objects/GameObject.h>

namespace WxEditor
{
using namespace GameEngine;

NewScene::NewScene()
    : Scene("NewScene")
{
}
int NewScene::Initialize()
{
    addPrimitive(PrimitiveType::Plane, vec3(0.f, 0.f, 0.f), vec3(10.f, 1.f, 10.f));
    addPrimitive(PrimitiveType::Cube, vec3(0.f, 1.0f, 0.f));
    addDefaultLight();
    addDefalutCamera();
    return 0;
}
void NewScene::addPrimitive(PrimitiveType type, const vec3& pos, const vec3& scale)
{
    auto model = resourceManager_->GetPrimitives(type);
    auto obj   = CreateGameObject(std::string(magic_enum::enum_name(type)));
    obj->AddComponent<Components::RendererComponent>().AddModel(model);
    obj->SetLocalPosition(pos);
    obj->SetLocalScale(scale);
    AddGameObject(std::move(obj));
}
void NewScene::addDefaultLight()
{
    auto directionLightGo = CreateGameObject("DirectionLight");
    directionLightGo->AddComponent<Components::DirectionalLightComponent>();
    directionLightGo->SetWorldPosition(vec3(1000, 1500, 1000));
    AddGameObject(std::move(directionLightGo));
}
void NewScene::addDefalutCamera()
{
    auto defaultSceneCamera    = CreateGameObject("DefaultCamera");
    auto& cameraComponent      = defaultSceneCamera->AddComponent<Components::CameraComponent>();
    cameraComponent.settings   = Components::CameraComponent::Settings::GlobalConfig;
    cameraComponent.mainCamera = true;
    defaultSceneCamera->SetWorldPosition(vec3(2, 2, 2));
    cameraComponent.LookAt(vec3(0));
    AddGameObject(std::move(defaultSceneCamera));
}
void NewScene::PostInitialize()
{
}
int NewScene::Update(float)
{
    return 0;
}
}  // namespace WxEditor
