#pragma once
#include "ISelectState.h"
#include "TestGame/MRpg/Scenes/SelectCharacter/CharacterSlot.h"

namespace GameEngine
{
namespace Renderer
{
class RenderersManager;
}  // namespace Renderer
}  // namespace GameEngine

namespace common
{
class Dispacher;
}  // namespace common

namespace Network
{
class CGateway;
}  // namespace Network

namespace MmmoRpg
{
class GetCharacterState : public ISelectState
{
public:
    GetCharacterState(common::Dispacher& dispatcher, GameEngine::Renderer::RenderersManager& rendererManager, Network::CGateway& gateway, std::vector<CharacterSlot>& charactersData);
    virtual ~GetCharacterState() override;
    virtual void Update() override;

private:
    void AddSlot(const Network::CharacterInfo& info);
    void SlotToRenderer(const CharacterSlot& slot, uint32 id);
    void GetCharacters(const std::vector<Network::CharacterInfo>& characters);
    void SendGetCharacter();

private:
    common::Dispacher& dispatcher_;
    Network::CGateway& gateway_;
    GameEngine::Renderer::RenderersManager& rendererManager_;
    std::vector<CharacterSlot>& charactersData_;
    vec3 itemsTextColour_;
    Timepoint sentTime_;
};
}  // namespace MmmoRpg
