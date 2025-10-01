#pragma once
#include "BidirectionalUnorderedMap.h"
#include "CharacterActions.h"
#include "Common/Messages/TransformMessages/TransformMessageTypes.h"

namespace common
{
namespace Controllers
{
typedef BidirectionalUnorderedMap<TransformMessageTypes, common::Controllers::CharacterActions::Type> CharacterControllerTypesMap;

class NetworkActionsConverter
{
public:
    static TransformMessageTypes Convert(CharacterActions::Type);
    static CharacterActions::Type Convert(TransformMessageTypes);

private:
    static CharacterControllerTypesMap characterControllerTypesMap_;
};
}  // namespace Controllers
}  // namespace common
