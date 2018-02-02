#pragma once
#include "../IHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class GetCharactersDataHandler : public IHandler
		{
		public:
			GetCharactersDataHandler(Context& context)
				: IHandler({ Network::MessageTypes::GetCharactersDataReq, Network::MessageTypes::GetCharacterDataReq })
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
