#pragma once
#include "Types.h"

namespace MmmoRpg
{
enum SelectedCharacterState
{
    NOT_SET,
    NOT_CONNECTED_WITH_PLAYER,
    READY_TO_USE
};
struct MrpgGameContext
{
    std::pair<uint32, SelectedCharacterState> selectedCharacterId;
};
}  // namespace MmmoRpg
