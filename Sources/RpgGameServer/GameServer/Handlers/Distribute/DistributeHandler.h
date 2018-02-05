#pragma once
#include "Common/MessageHandling/AbstractHandler.h"
#include "../../Context.h"

namespace GameServer
{
	namespace Handler
	{
		class DistributeHandler : public common::AbstractHandler
		{
		public:
			DistributeHandler(Context& context)
				: common::AbstractHandler({Network::MessageTypes::Any})
				, context_(context)
			{}
		protected:
			virtual void ProcessMessage(const Network::BoxMessage& message) override;

		private:
			Context& context_;
		};
	} // Handler
} // GameServer
