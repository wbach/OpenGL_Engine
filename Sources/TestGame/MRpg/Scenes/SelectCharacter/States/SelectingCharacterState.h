#pragma once
#include "ISelectState.h"
#include "TestGame/MRpg/Scenes/SelectCharacter/CharacterSlot.h"
#include "optional.hpp"

namespace GameEngine
{
class InputManager;
namespace Renderer
{
class RenderersManager;
}  // Renderer
}  // GameEngine

namespace common
{
class Dispacher;
}  // common

namespace MmmoRpg
{
struct MrpgGameContext;

class SelectingCharacterState : public ISelectState
{
public:
    SelectingCharacterState(GameEngine::InputManager& inputManager, GameEngine::Renderer::RenderersManager& rendererManager, MrpgGameContext& gameContext, std::vector<CharacterSlot>& charactersData);
    virtual ~SelectingCharacterState() override;
    virtual void Update() override;

private:
    void SubscribesForKeysInput();
    void CheckRangeSelectedCharacter();
    void ChangeSelectedCharacter(int8 dir);
    void SetCurrentChoiceText();

private:
    GameEngine::InputManager& inputManager_;
    GameEngine::Renderer::RenderersManager& rendererManager_;
    MrpgGameContext& gameContext_;
    std::vector<CharacterSlot>& charactersData_;

    GameEngine::GuiTextElement characterSelectText_;
    wb::optional<int8> currentSelectCharacterId_;
};
}
