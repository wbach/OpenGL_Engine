#pragma once
#include "ISelectState.h"
#include "TestGame/MRpg/Scenes/SelectCharacter/CharacterSlot.h"
#include "optional.hpp"

namespace Input
{
    class InputManager;
} // Input

namespace GameEngine
{
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
    SelectingCharacterState(Input::InputManager& inputManager, GameEngine::Renderer::RenderersManager& rendererManager, MrpgGameContext& gameContext, std::vector<CharacterSlot>& charactersData);
    virtual ~SelectingCharacterState() override;
    virtual void Update() override;

private:
    void SubscribesForKeysInput();
    void CheckRangeSelectedCharacter();
    void ChangeSelectedCharacter(int8 dir);
    void SetCurrentChoiceText();

private:
    Input::InputManager& inputManager_;
    GameEngine::Renderer::RenderersManager& rendererManager_;
    MrpgGameContext& gameContext_;
    std::vector<CharacterSlot>& charactersData_;

    //GameEngine::GuiTextElement characterSelectText_;
    std::optional<int8> currentSelectCharacterId_;
};
}
