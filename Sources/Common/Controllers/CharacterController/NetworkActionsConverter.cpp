#include "NetworkActionsConverter.h"
namespace common
{
	namespace Controllers
	{
		CharacterControllerTypesMap NetworkActionsConverter::characterControllerTypesMap_ = CharacterControllerTypesMap
		({
			{ Network::TransformMessageTypes::MOVE_FORWARD,  CharacterActions::MOVE_FORWARD },
			{ Network::TransformMessageTypes::MOVE_BACKWARD, CharacterActions::MOVE_BACKWARD },
			{ Network::TransformMessageTypes::JUMP,			 CharacterActions::JUMP },
			{ Network::TransformMessageTypes::ROTATE_LEFT,   CharacterActions::ROTATE_LEFT },
			{ Network::TransformMessageTypes::ROTATE_RIGHT,  CharacterActions::ROTATE_RIGHT },
		});

		Network::TransformMessageTypes NetworkActionsConverter::Convert(CharacterActions::Type type)
		{
			return characterControllerTypesMap_[type];
		}

		CharacterActions::Type NetworkActionsConverter::Convert(Network::TransformMessageTypes type)
		{
			return characterControllerTypesMap_[type];
		}
	} // Controllers
} // common