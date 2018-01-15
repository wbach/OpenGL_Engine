#include "ConnectionManager.h"
#include "Logger/Log.h"
#include "Reciever.h"

namespace Network
{
	ConnectionManager::ConnectionManager(ISDLNetWrapper* sdlNetWrapper, ConectContext& context)
		: context_(context)
		, clientsCount_(0)
		, sdlNetWrapper_(sdlNetWrapper)
		, sender_(sdlNetWrapper)
	{
		usersDb_ = {
			{"baszek", "haslo"}
		};
	}

	void ConnectionManager::CheckNewConnectionsToServer()
	{
		if (!CheckSocketsActivity())
			return;

		CreateClientSocketIfAvailable();
		DissmissConection();
	}

	void ConnectionManager::WaitForAuthentication()
	{
		for (auto iter = notAuthenticatedUsers.begin(); iter != notAuthenticatedUsers.end(); )
		{
			if (!ProccessAuthentication(iter))
				++iter;
		}
	}

	bool ConnectionManager::ProccessAuthentication(Users::iterator& userIter)
	{
		Receiver receiver;

		auto& user = userIter->second;

		auto msg = receiver.Receive(user->socket);

		if (msg == nullptr)
			return false;

		if (msg->GetType() != MessageTypes::Authentication)
			return false;

		auto amsg = dynamic_cast<AuthenticationMessage*>(msg.get());

		if (amsg == nullptr)
			return false;

		auto name = amsg->GetUserName();
		auto pass = amsg->GetPassword();

		//Log("User login: " + name + "\nPassword: " + pass);

		if (usersDb_[name] == pass)
		{
			ConnectionMessage conMsg(ConnectionStatus::CONNECTED);
			sender_.SendTcp(user->socket, &conMsg);

			context_.users[user->id] = user;

			for (auto s : newUserSubscribes_)
				s(user->id);

			Log(name + " connected. There are now " + std::to_string(clientsCount_) + " client(s) connected.");
		}
		else
		{
			ConnectionMessage conMsg(ConnectionStatus::ERROR_FAILD_AUTHENTICATION);
			sender_.SendTcp(user->socket, &conMsg);
			--clientsCount_;
			Log("Wrong username or password for " + name + ". Disconnected. There are now " + std::to_string(clientsCount_) + " client(s) connected.");
		}

		userIter = notAuthenticatedUsers.erase(userIter);

		return true;
	}

	void ConnectionManager::CreateClientSocketIfAvailable()
	{
		if (!IsSpace())
			return;

		auto usr = std::make_shared<UtilsNetwork::UserData>();

		usr->socket = sdlNetWrapper_->TCPAccept(context_.socket);

		sdlNetWrapper_->TCPAddSocket(context_.socketSet, usr->socket);		

		notAuthenticatedUsers[usr->id] = usr;

		++clientsCount_;

		ConnectionMessage conMsg(ConnectionStatus::WAIT_FOR_AUTHENTICATION);
		sender_.SendTcp(usr->socket, &conMsg);

		/*std::string ok = "OK";
		sdlNetWrapper_->SendTcp(usr->socket, (void *)ok.c_str(), ok.size() + 1);*/

		Log("Client connected. Wait for authentication. There are now " + std::to_string(clientsCount_) + " client(s) connected.");
		//Log("Client connected. There are now " + std::to_string(clientsCount_) + " client(s) connected.");
	}

	bool ConnectionManager::CheckSocketsActivity()
	{		
		int numActiveSockets = sdlNetWrapper_->CheckSockets(context_.socketSet, 0);

/*		if (numActiveSockets == 0)
			return false;	*/	

		WaitForAuthentication();

		if (sdlNetWrapper_->SocketReady((SDLNet_GenericSocket) context_.socket) == 0)
			return false;

		return true;
	}

	void ConnectionManager::SubscribeForNewUser(CreationFunc func)
	{
		newUserSubscribes_.push_back(func);
	}

	void ConnectionManager::SubscribeForDisconnectUser(CreationFunc func)
	{
	}

	void ConnectionManager::DissmissConection()
	{
		if (IsSpace())
			return;

		Log("*** Maximum client count reached - rejecting client connection ***");

		TCPsocket tempSock = sdlNetWrapper_->TCPAccept(context_.socket);

		ConnectionMessage conMsg(ConnectionStatus::ERROR_FULL);
		sender_.SendTcp(tempSock, &conMsg);

		/*std::string full = "FULL";
		sdlNetWrapper_->SendTcp(tempSock, (void *)full.c_str(), full.size() + 1);*/
		sdlNetWrapper_->TCPClose(tempSock);
	}

	bool ConnectionManager::IsSpace() const
	{
		return clientsCount_ < context_.maxClients;
	}
}