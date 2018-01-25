#pragma once
#include "../IHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class GetCharactersHandler : public IHandler
		{
		public:
			GetCharactersHandler(Context& context)
				: IHandler({ Network::MessageTypes::GetCharactersReq })
				, context_(context)
			{}
		protected:
			virtual void ProcessMessage(const Network::BoxMessage& message) override;

		private:
			Context & context_;
		};
	} // Handler
} // GameServer
