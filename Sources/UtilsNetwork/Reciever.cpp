#include "Reciever.h"
#include "Logger/Log.h"
#include "GLM/GLMUtils.h"
#include "Message.h"

namespace Network
{
	Receiver::Receiver(ISDLNetWrapper* sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
	{
	}

	std::shared_ptr<IMessage> Receiver::Receive(TCPsocket socket)
	{
		std::shared_ptr<IMessage> result;

		//char buffer[512];
		MessageHeader header;
		int recvBytes = sdlNetWrapper_->RecvTcp(socket, &header, sizeof(header));
		Log(std::string("Receive header, msg type : ") + std::to_string(header.msgType));

		if (recvBytes <= 0)
			return nullptr;

		switch (header.msgType)
		{
		case MessageTypes::ConnectionMsg:	result = GetIMessage<ConnectionMessage>(socket);		break;
		case MessageTypes::TestDataMsg:		result = GetIMessage<TestData>(socket);					break;
		case MessageTypes::Authentication:	result = GetIMessage<AuthenticationMessage>(socket);	break;
		}
		return result;
	}
	//std::shared_ptr<IMessage> Receiver::GetConnectionMessage(TCPsocket socket)
	//{
	//	ConnectionMessage connectionMsg;
	//	auto recvBytes = sdlNetWrapper_->RecvTcp(socket, &connectionMsg, sizeof(connectionMsg));
	//	Log("Receive connectionMsg, connection status: " + std::to_string(connectionMsg.connectionStatus));

	//	return std::make_shared<ConnectionMessage>(connectionMsg);
	//}
	//std::shared_ptr<IMessage> Receiver::GetTestDataMessage(TCPsocket socket)
	//{
	//	TestData data;
	//	auto recvBytes = sdlNetWrapper_->RecvTcp(socket, &data, sizeof(data));

	//	if (recvBytes <= 0)
	//		return nullptr;

	//	Log(std::string("Receive TestDataMsg : \nPos: ") + Utils::ToString(data.position) + "\nRot: " + Utils::ToString(data.rotation) + "\nAction :" + std::to_string(data.type) /*+ "\nMSG: " + data.str*/ + "\n ID : " + std::to_string(data.id));
	//	return std::make_shared<TestData>(data);
	//}
	//std::shared_ptr<IMessage> Receiver::GetAuthenticationMessage(TCPsocket socket)
	//{
	//	AuthenticationMessage authMsg;
	//	auto recvBytes = sdlNetWrapper_->RecvTcp(socket, &authMsg, sizeof(authMsg));
	//	Log("Receive AuthenticationMessage, username : " + authMsg.GetUserName());
	//	return std::make_shared<TestData>(authMsg);
	//}


} // Network