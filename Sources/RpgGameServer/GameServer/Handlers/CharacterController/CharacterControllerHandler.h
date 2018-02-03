#pragma once
#include "../IHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class CharacterControllerHandler : public IHandler
		{
		public:
			CharacterControllerHandler(Context& context);

		protected:
			virtual void ProcessMessage(const Network::BoxMessage& message) override;

		private:
			Context& context_;
		};
	} // Handler
} // GameServer
