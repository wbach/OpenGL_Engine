#pragma once
#include "../IHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class SelectCharacterHandler : public IHandler
		{
		public:
			SelectCharacterHandler(Context& context)
				: IHandler({ Network::MessageTypes::SelectCharacterReq })
				, context_(context)
			{}
		protected:
			virtual void ProcessMessage(const Network::BoxMessage& message) override;
			
		private:
			void SendResponse(bool status, uint32 userId, uint32 characterId);

		private:
			Context & context_;
		};
	} // Handler
} // GameServer
