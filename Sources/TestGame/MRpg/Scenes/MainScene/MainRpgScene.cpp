#include "MainRpgScene.h"

#include <Common/Messages/GetCharacterData/GetCharactersDataMsgReq.h>
#include <Common/Messages/RemoveCharacter/DisconnectCharacterMsg.h>
#include <Common/Messages/TransformMessages/TransformMessageTypes.h>
#include <GameEngine/Engine/EngineEvent.h>
#include <Input/InputManager.h>

#include "Camera/FirstPersonCamera.h"
#include "Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Renderers/RenderersManager.h"
#include "SingleTon.h"
#include "TestGame/MRpg/Characters/PlayerController.h"
#include "TestGame/MRpg/Handlers/DisconnectCharacter/DisconnectHandler.h"
#include "TestGame/MRpg/Handlers/GetCharacterData/GetCharacterDataHandler.h"
#include "TestGame/MRpg/Handlers/Transform/TransformHandler.h"

using namespace GameEngine;

namespace MmmoRpg
{
MainRpgScene::MainRpgScene(Network::Gateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext)
    : MRpgScene("MainRpgScene", gateway, serverAddress, gameContext)
{
}

MainRpgScene::~MainRpgScene()
{
    DEBUG_LOG("");

    if (cameraId_)
        camera.remove(*cameraId_);
}

int MainRpgScene::Initialize()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(GameEngine::EngineEvent::QUIT); });

    DEBUG_LOG("MainRpgScene::Initialize()");

    modelsCreator_ = std::make_unique<ModelsCreator>(resourceManager_.get());
    networkCharacterManager_ =
        std::make_unique<NetworkCharacterManager>(modelsCreator_.get(), *renderersManager_, gameContext_,
                                                  std::bind(&MainRpgScene::AddGameObject, this, std::placeholders::_1),
                                                  std::bind(&MainRpgScene::CreateGameObjectWithDefaultName, this));

    networkCharacterManager_->SubscribeOnGetPlayer(std::bind(&MainRpgScene::OnGetPlayer, this, std::placeholders::_1));

    dispatcher_.AddHandler("GetCharacterDataHandler", new GetCharacterDataHandler(*networkCharacterManager_));
    dispatcher_.AddHandler("DisconnectHandler", new DisconnectHandler(*networkCharacterManager_));
    dispatcher_.AddHandler("TransformHandler", new TransformHandler(*networkCharacterManager_));

    ReqNetworkSceneCharacters();

    auto obj = CreateGameObject();
    obj->GetTransform().SetPosition(vec3(100, 17, -7));
    obj->AddComponent<Components::RendererComponent>().AddModel("Meshes/Bialczyk/Bialczyk.obj");
    AddGameObject(std::move(obj));

    // camera = std::make_unique<CFirstPersonCamera>(inputManager_, displayManager_);
    playerController_ = std::make_shared<PlayerController>(inputManager_, gameContext_, gateway_);

    dayNightCycle.SetDirectionalLight(&directionalLight);
    dayNightCycle.SetTime(.5f);
    return 0;
}

int MainRpgScene::Update(float dt)
{
    gateway_.Update();
    networkCharacterManager_->Update(dt);
    dayNightCycle.Update(dt);
    UpdatePlayerandCamera(dt);
    return 0;
}

void MainRpgScene::UpdatePlayerandCamera(float)
{
    camera.CalculateInput();
    camera.Move();
}

std::unique_ptr<GameEngine::GameObject> MainRpgScene::CreateGameObjectWithDefaultName()
{
    return CreateGameObject();
}

void MainRpgScene::ReqNetworkSceneCharacters()
{
    DEBUG_LOG("Send request");
    common::GetCharactersDataMsgReq characterMsgReq;
    characterMsgReq.mapId = 1;  // hack id : 1
    gateway_.Send(characterMsgReq);
}

void MainRpgScene::OnGetPlayer(NetworkCharacter* character)
{
    auto tcamera =
        std::make_unique<GameEngine::ThirdPersonCamera>(*inputManager_, character->GetGameObject().GetTransform());
    cameraId_ = camera.addAndSet(std::move(tcamera));
}

/*void MainRpgScene::HandleTransformMsg(std::shared_ptr<Network::TransformMsgResp> msg)
{
    DEBUG_LOG("Times test : Resp: " + std::to_string(clock() * 1000.0f / (float)CLOCKS_PER_SEC) + " Action: " +
std::to_string(msg->action));

    auto characterId = msg->id;
    DEBUG_LOG("msg->position : " + Utils::ToString(msg->position) + " msg->rotation : " +
Utils::ToString(msg->rotation)); networkCharacters_[msg->id]->GetEntity()->worldTransform.SetPosition(msg->position);
    networkCharacters_[msg->id]->GetEntity()->worldTransform.SetRotation(msg->rotation);
    auto icontroller =
networkCharacters_[msg->id]->GetControllerByType(common::Controllers::Types::CharacterControllerType);
    auto controller = common::Controllers::castControllerAs<common::Controllers::CharacterController>(icontroller);
    DEBUG_LOG("Controller->position : " + Utils::ToString(controller->GetTransform().GetPosition()) + "
Controller->rotation : " + Utils::ToString(controller->GetTransform().GetRotation()));

    switch (msg->action)
    {
    case Network::TransformAction::PUSH:
        controller->AddState(common::Controllers::NetworkActionsConverter::Convert(msg->type));
        break;
    case Network::TransformAction::POP:
        controller->RemoveState(common::Controllers::NetworkActionsConverter::Convert(msg->type));
        break;
    }
}*/
}  // namespace MmmoRpg
