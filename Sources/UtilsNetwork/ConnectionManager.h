#pragma once
#include <functional>
#include <unordered_map>
#include "ConectContext.h"
#include "ISDLNetWrapper.h"
#include "Reciever.h"
#include "Sender.h"
#include "Types.h"
#include "UserAccount.h"

namespace Network
{
typedef std::function<void(const std::string&, uint32)> CreationFunc;

class ConnectionManager
{
public:
    ConnectionManager(Sender& sender, Receiver& receiver, ISDLNetWrapper& sdlNetWrapper, ConectContext& context);
    void CheckNewConnectionsToServer();
    bool CheckSocketsActivity();
    void SubscribeForNewUser(CreationFunc func);
    void SubscribeForDisconnectUser(CreationFunc func);
    void DisconectUser(uint32 id);

private:
    void WaitForAuthentication();
    bool IsSomthingOnServerSocket();
    bool ProccessAuthentication(Users::iterator& userIter);
    void CreateClientSocketIfAvailable();
    void DissmissConection();
    bool IsSpace() const;

private:
    ConectContext& context_;
    uint32 clientsCount_;
    ISDLNetWrapper& sdlNetWrapper_;
    Sender& sender_;
    Receiver& receiver_;
    Users notAuthenticatedUsers;
    std::vector<CreationFunc> newUserSubscribes_;

    // tmp simple dataBase
    std::unordered_map<std::string, UserAccount> usersDb_;
};
}  // namespace Network
