#pragma once
#include "CharacterSlot.h"
#include "NetworkTypes.h"
#include "States/ISelectState.h"
#include "TestGame/MRpg/Scenes/MRpgScene.h"

namespace MmmoRpg
{
class SelectCharacterScene : public MRpgScene
{
public:
    SelectCharacterScene(Network::Gateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext);
    virtual ~SelectCharacterScene() override;
    virtual int Initialize() override;
    virtual void PostInitialize() override;
    virtual int Update(float deltaTime) override;

private:
    void CreateState();

private:
    uint32 currentState_;
    std::unique_ptr<ISelectState> state_;
    std::vector<CharacterSlot> charactersData_;
    Timepoint sentTime_;
};
}
