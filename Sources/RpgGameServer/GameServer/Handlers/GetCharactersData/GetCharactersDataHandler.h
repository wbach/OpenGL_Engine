#pragma once
#include "Common/MessageHandling/AbstractHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class GetCharactersDataHandler : public common::AbstractHandler
		{
		public:
			GetCharactersDataHandler(Context& context)
				: common::AbstractHandler({ Network::MessageTypes::GetCharactersDataReq, Network::MessageTypes::GetCharacterDataReq })
				, context_(context)
			{}
		protected:
			virtual void ProcessMessage(const Network::BoxMessage& message) override;
			void ProccesGetCharactersDataReq(const Network::BoxMessage& message);
			void ProccesGetCharacterDataReq(const Network::BoxMessage& message);

		private:
			Context & context_;
		};
	} // Handler
} // GameServer
