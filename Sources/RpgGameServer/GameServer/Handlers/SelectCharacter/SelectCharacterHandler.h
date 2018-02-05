#pragma once
#include "Common/MessageHandling/AbstractHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class SelectCharacterHandler : public common::AbstractHandler
		{
		public:
			SelectCharacterHandler(Context& context)
				: common::AbstractHandler({ Network::MessageTypes::SelectCharacterReq })
				, context_(context)
			{}
		protected:
			virtual void ProcessMessage(const Network::BoxMessage& message) override;
			
		private:
			void SendResponse(bool status, uint32 userId, uint32 characterId, uint32 mapId);

		private:
			Context & context_;
		};
	} // Handler
} // GameServer
