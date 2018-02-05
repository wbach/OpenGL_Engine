#pragma once
#include "Common/MessageHandling/AbstractHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class CharacterControllerHandler : public common::AbstractHandler
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
