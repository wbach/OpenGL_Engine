#include "ConnectionManager.h"
#include "Logger/Log.h"
#include "Messages/Conntection/AuthenticationMessage.h"
#include "Messages/Conntection/ConnectionMessage.h"

namespace Network
{
ConnectionManager::ConnectionManager(Sender& sender, Receiver& receiver, ISDLNetWrapper &sdlNetWrapper, ConectContext& context)
    : context_(context)
    , clientsCount_(0)
    , sdlNetWrapper_(sdlNetWrapper)
    , sender_(sender)
    , receiver_(receiver)
{
    // clang-format off
    usersDb_ = {
        {"baszek", UserAccount{14, "baszek", "haslo"}},
        {"baszeka", UserAccount{7, "baszeka", "haslo"}},
        {"baszekb", UserAccount{55, "baszekb", "haslo"}},
        {"baszekb", UserAccount{21, "baszekc", "haslo"}},
        {"baszekd", UserAccount{13, "baszekd", "haslo"}},
    };
    // clang-format on

    for (uint32 x = 100; x < 200; ++x)
    {
        auto name      = "mock_" + std::to_string(x);
        usersDb_[name] = UserAccount{x, "name", "haslo"};
    }
}

void ConnectionManager::CheckNewConnectionsToServer()
{
    WaitForAuthentication();

    if (!IsSomthingOnServerSocket())
        return;

    CreateClientSocketIfAvailable();
    DissmissConection();
}

void ConnectionManager::WaitForAuthentication()
{
    for (auto iter = notAuthenticatedUsers.begin(); iter != notAuthenticatedUsers.end();)
    {
        if (!ProccessAuthentication(iter))
            ++iter;
    }
}

bool ConnectionManager::IsSomthingOnServerSocket()
{
    return sdlNetWrapper_.SocketReady((SDLNet_GenericSocket)context_.socket) > 0;
}

bool ConnectionManager::ProccessAuthentication(Users::iterator& userIter)
{
    auto& user = userIter->second;


    auto [status, msg] = receiver_.Receive(user->socket);

    if (msg == nullptr)
        return false;

    if (msg->GetType() != MessageTypes::Authentication)
        return false;

    auto amsg = castMessageAs<AuthenticationMessage>(msg.get());

    if (amsg == nullptr)
        return false;

    auto name            = amsg->GetUserName();
    auto pass            = amsg->GetPassword();
    auto connectedUserId = usersDb_[name].id;
    bool authenticated   = true;

    ConnectionStatus errorConnectionStatus = ConnectionStatus::ERROR_FAILD_AUTHENTICATION;
    std::string errorString                = "Wrong username or password for ";
    if (usersDb_.count(name) == 0)
        authenticated = false;

    if (usersDb_[name].password != pass)
        authenticated = false;

    for (const auto& existingUser : context_.users)
    {
        if (existingUser.second->id == connectedUserId)
        {
            errorConnectionStatus = ConnectionStatus::ACCOUNT_IN_USE;
            authenticated         = false;
            errorString           = "Account in use : ";
            break;
        }
    }

    if (authenticated)
    {
        ConnectionMessage conMsg(ConnectionStatus::CONNECTED);
        sender_.SendTcp(user->socket, conMsg);

        user->id                        = connectedUserId;
        context_.users[connectedUserId] = user;

        for (auto s : newUserSubscribes_)
            s(name, connectedUserId);

        DEBUG_LOG(name + " connected. There are now " + std::to_string(clientsCount_) + " client(s) connected.");
    }
    else
    {
        ConnectionMessage conMsg(errorConnectionStatus);
        sender_.SendTcp(user->socket, conMsg);
        --clientsCount_;
        DEBUG_LOG(errorString + name + ". Disconnected. There are now " + std::to_string(clientsCount_) + " client(s) connected.");
    }

    userIter = notAuthenticatedUsers.erase(userIter);

    return true;
}

void ConnectionManager::CreateClientSocketIfAvailable()
{
    if (!IsSpace())
        return;

    auto usr = std::make_shared<UtilsNetwork::UserData>();

    usr->socket = sdlNetWrapper_.TCPAccept(context_.socket);

    sdlNetWrapper_.TCPAddSocket(context_.socketSet, usr->socket);

    notAuthenticatedUsers[usr->GetNextId()] = usr;

    ++clientsCount_;

    ConnectionMessage conMsg(ConnectionStatus::WAIT_FOR_AUTHENTICATION);
    sender_.SendTcp(usr->socket, conMsg);

    DEBUG_LOG("Client connected. Wait for authentication. There are now " + std::to_string(clientsCount_) + " client(s) connected.");
}

bool ConnectionManager::CheckSocketsActivity()
{
    return (sdlNetWrapper_.CheckSockets(context_.socketSet, 0)) > 0;
}

void ConnectionManager::SubscribeForNewUser(CreationFunc func)
{
    newUserSubscribes_.push_back(func);
}

void ConnectionManager::SubscribeForDisconnectUser(CreationFunc)
{
}

void ConnectionManager::DisconectUser(uint32 id)
{
    sdlNetWrapper_.TCPCloseAndDeleteSocket(context_.socketSet, context_.users[id]->socket);
    --clientsCount_;
    DEBUG_LOG("User disconnected. There are now " + std::to_string(clientsCount_) + " client(s) connected.");
}

void ConnectionManager::DissmissConection()
{
    if (IsSpace())
        return;

    DEBUG_LOG("*** Maximum client count reached - rejecting client connection ***");

    TCPsocket tempSock = sdlNetWrapper_.TCPAccept(context_.socket);

    ConnectionMessage conMsg(ConnectionStatus::ERROR_FULL);
    sender_.SendTcp(tempSock, conMsg);

    /*std::string full = "FULL";
    sdlNetWrapper_.SendTcp(tempSock, (void *)full.c_str(), full.size() + 1);*/

    sdlNetWrapper_.TCPCloseAndDeleteSocket(context_.socketSet, tempSock);
}

bool ConnectionManager::IsSpace() const
{
    return clientsCount_ < context_.maxClients;
}
}  // namespace Network
