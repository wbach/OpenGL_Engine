#pragma once
#include "Common/MessageHandling/AbstractHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class GetCharactersHandler : public common::AbstractHandler
		{
		public:
			GetCharactersHandler(Context& context)
				: common::AbstractHandler({ Network::MessageTypes::GetCharactersReq })
				, context_(context)
			{}
		protected:
			virtual void ProcessMessage(const Network::BoxMessage& message) override;

		private:
			Context & context_;
		};
	} // Handler
} // GameServer
