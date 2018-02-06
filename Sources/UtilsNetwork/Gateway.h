#pragma once
#include "Types.h"
#include "Sender.h"
#include "Reciever.h"
#include "ConectContext.h"
#include "ConnectionManager.h"
#include "ServerCreator.h"
#include "ClientCreator.h"
#include "NetworkTypes.h"
#include "Time/TimeMeasurer.h"
#include <vector>
#include <unordered_map>

namespace Network
{
	typedef std::function<void(uint32)> DisconectFunc;

	class CGateway
	{
	public:
		CGateway();
		CGateway(Utils::Time::CTimeMeasurer timeMeasurer);
		virtual ~CGateway();
		void StartServer(uint32 maxClients, uint32 port);
		bool ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port);
		void SubscribeForNewUser(CreationFunc func);
		void SubscribeForDisconnectUser(DisconectFunc func);
		void SubscribeOnMessageArrived(const std::string& label, OnMessageArrived func, MessageTypes messageType = MessageTypes::Any);
		void UnsubscrieOnMessageArrived(const std::string& label);
		void UnsubscribeAllOnMessageArrived();

		bool Send(IMessage* message);
		bool Send(uint32 userId, IMessage* message);
		void Update();

	protected:
		void DisconnectUser(uint32 id);

	protected:
		Utils::Time::CTimeMeasurer timeMeasurer_;
		ISDLNetWrapperPtr iSDLNetWrapperPtr_;

		Sender sender_;
		Receiver receiver_;
		ConectContext context_;
		ConnectionManager connectionManager_;

		ServerCreator serverCreator_;
		ClientCreator clientCreator_;

		std::unordered_map<std::string, std::pair<MessageTypes, OnMessageArrived>> onMessageArrivedSubcribes_;
		std::vector<DisconectFunc> disconnectSubscribes_;

		bool isServer;
		bool running;
	};

	typedef std::shared_ptr<CGateway> GatewayPtr;
}