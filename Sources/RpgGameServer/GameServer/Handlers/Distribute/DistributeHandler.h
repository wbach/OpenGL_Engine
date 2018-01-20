#pragma once
#include "../IHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class DistributeHandler : public IHandler
		{
		public:
			DistributeHandler(Context& context)
				: IHandler({Network::MessageTypes::Any})
				, context_(context)
			{}
		protected:
			virtual void ProcessMessage(const Network::BoxMessage& message) override;

		private:
			Context& context_;
		};
	} // Handler
} // GameServer
