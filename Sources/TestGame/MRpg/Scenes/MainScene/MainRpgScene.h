#pragma once
#include "Gateway.h"
#include "TestGame/MRpg/Characters/ModelsCreator.h"
#include "TestGame/MRpg/Characters/NetworkCharacterManger.h"
#include "TestGame/MRpg/MrpgGameContext.h"
#include "TestGame/MRpg/Scenes/MRpgScene.h"

namespace MmmoRpg
{
class PlayerController;

class MainRpgScene : public MRpgScene
{
public:
    MainRpgScene(Network::CGateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext);
    virtual ~MainRpgScene() override;
    virtual int Initialize() override;
    virtual void PostInitialize() override{}
    virtual int Update(float deltaTime) override;

private:
    void UpdatePlayerandCamera(float time);
    std::unique_ptr<GameEngine::GameObject> CreateGameObjectWithDefaultName();

private:
    void ReqNetworkSceneCharacters();
    void OnGetPlayer(NetworkCharacter* character);

private:
    std::shared_ptr<PlayerController> playerController_;
    NetworkCharacterManagerPtr networkCharacterManager_;
    ModelsCreatorPtr modelsCreator_;
    common::Transform cameraTarget_;
};
}  // MmmoRpg
