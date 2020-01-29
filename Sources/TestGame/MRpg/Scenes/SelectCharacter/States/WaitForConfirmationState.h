#pragma once
#include "GameEngine/Scene/SceneEvents.h"
#include "ISelectState.h"
#include "TestGame/MRpg/Scenes/SelectCharacter/CharacterSlot.h"

namespace GameEngine
{
namespace Renderer
{
class RenderersManager;
}  // namespace Renderer
}  // namespace GameEngine

namespace Network
{
class Gateway;
}  // namespace Network

namespace common
{
class Dispacher;
}  // namespace common

namespace MmmoRpg
{
struct MrpgGameContext;

class WaitForConfirmationState : public ISelectState
{
public:
    WaitForConfirmationState(common::Dispacher& dispatcher, Network::Gateway& gateway, MrpgGameContext& gameContext, GameEngine::AddEvent& addEvent, std::vector<CharacterSlot>& charactersData);
    virtual ~WaitForConfirmationState() override;
    virtual void Update() override;

private:
    void SceneToLoad(uint32 characterId, uint32 mapId, bool success);
    void SendSelectCharacterReq();

private:
    common::Dispacher& dispatcher_;
    Network::Gateway& gateway_;
    MrpgGameContext& gameContext_;
    std::vector<CharacterSlot>& charactersData_;
    GameEngine::AddEvent addSceneEvent;
    Timepoint sentTime_;
};
}  // namespace MmmoRpg
