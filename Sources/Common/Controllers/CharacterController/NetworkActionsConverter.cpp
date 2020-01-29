#include "NetworkActionsConverter.h"

namespace common
{
namespace Controllers
{
CharacterControllerTypesMap NetworkActionsConverter::characterControllerTypesMap_ = CharacterControllerTypesMap({
    {TransformMessageTypes::MOVE_FORWARD, CharacterActions::MOVE_FORWARD},
    {TransformMessageTypes::MOVE_BACKWARD, CharacterActions::MOVE_BACKWARD},
    {TransformMessageTypes::JUMP, CharacterActions::JUMP},
    {TransformMessageTypes::ROTATE_LEFT, CharacterActions::ROTATE_LEFT},
    {TransformMessageTypes::ROTATE_RIGHT, CharacterActions::ROTATE_RIGHT},
});

TransformMessageTypes NetworkActionsConverter::Convert(CharacterActions::Type type)
{
    return characterControllerTypesMap_[type];
}

CharacterActions::Type NetworkActionsConverter::Convert(TransformMessageTypes type)
{
    return characterControllerTypesMap_[type];
}
}  // namespace Controllers
}  // namespace common
