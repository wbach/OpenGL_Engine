#include "SelectingCharacterState.h"
#include "Input/InputManager.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "TestGame/MRpg/MrpgGameContext.h"

namespace MmmoRpg
{
SelectingCharacterState::SelectingCharacterState(Input::InputManager& inputManager, GameEngine::Renderer::RenderersManager& rendererManager, MrpgGameContext& gameContext, std::vector<CharacterSlot>& charactersData)
    : inputManager_(inputManager)
    , rendererManager_(rendererManager)
    , gameContext_(gameContext)
    , charactersData_(charactersData)
{
    SubscribesForKeysInput();
}
SelectingCharacterState::~SelectingCharacterState()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::ENTER);
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::A);
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::D);
}
void SelectingCharacterState::Update()
{
    SetCurrentChoiceText();
}
void SelectingCharacterState::ChangeSelectedCharacter(int8 dir)
{
    if (!currentSelectCharacterId_)
    {
        currentSelectCharacterId_ = 0;
        return;
    }

    currentSelectCharacterId_.value() += dir;
}
void SelectingCharacterState::CheckRangeSelectedCharacter()
{
    if (!currentSelectCharacterId_)
        return;

    auto& v   = currentSelectCharacterId_.value();
    auto size = static_cast<uint8>(charactersData_.size());

    if (v >= size)
        v = size - 1;

    if (v < 0)
        v = 0;
}
void SelectingCharacterState::SetCurrentChoiceText()
{
    if (charactersData_.empty() || !currentSelectCharacterId_)
        return;

//    rendererManager_.GuiText("CurrentChoice")          = characterSelectText_;
 //   rendererManager_.GuiText("CurrentChoice").position = vec2(-0.2, 0);
 //   rendererManager_.GuiText("CurrentChoice").text     = charactersData_[currentSelectCharacterId_.value()].characterInfo.GetName();
}

void SelectingCharacterState::SubscribesForKeysInput()
{
    inputManager_.SubscribeOnKeyDown(KeyCodes::ENTER, [&]() {
        if (!currentSelectCharacterId_)
            return;

        gameContext_.selectedCharacterId = {currentSelectCharacterId_.constValue(), SelectedCharacterState::NOT_CONNECTED_WITH_PLAYER};
        status_                          = StateStatus::DONE;
    });

    inputManager_.SubscribeOnKeyDown(KeyCodes::A, [&]() {
        ChangeSelectedCharacter(-1);
        CheckRangeSelectedCharacter();
    });

    inputManager_.SubscribeOnKeyDown(KeyCodes::D, [&]() {
        ChangeSelectedCharacter(1);
        CheckRangeSelectedCharacter();
    });
}
}  // MmmoRpg
