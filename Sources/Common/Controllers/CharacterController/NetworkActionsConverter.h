#pragma once
#include "CharacterActions.h"
#include "../../../UtilsNetwork/Messages/TransformMessages/TransformMessageTypes.h"
#include "BidirectionalUnorderedMap.h"

namespace common
{
	namespace Controllers
	{
		typedef BidirectionalUnorderedMap<Network::TransformMessageTypes, common::Controllers::CharacterActions::Type> CharacterControllerTypesMap;

		class NetworkActionsConverter
		{
		public:
			static Network::TransformMessageTypes Convert(CharacterActions::Type);
			static CharacterActions::Type Convert(Network::TransformMessageTypes);

		private:
			static CharacterControllerTypesMap characterControllerTypesMap_;
		};
	} // Controllers
} // common