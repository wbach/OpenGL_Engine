#pragma once
#include <unordered_map>
#include <vector>

#include "ClientCreator.h"
#include "ConectContext.h"
#include "ConnectionManager.h"
#include "NetworkTypes.h"
#include "Reciever.h"
#include "Sender.h"
#include "ServerCreator.h"
#include "Time/TimeMeasurer.h"
#include "Types.h"

namespace Network
{
typedef std::function<void(uint32)> DisconectFunc;

class Gateway
{
public:
    Gateway();
    Gateway(Utils::Time::CTimeMeasurer timeMeasurer);
    virtual ~Gateway();
    bool StartServer(uint32 maxClients, uint32 port);
    bool ConnectToServer(const std::string& username, const std::string& password, const std::string& host,
                         uint32 port);
    void SubscribeForNewUser(CreationFunc func);
    void SubscribeForDisconnectUser(DisconectFunc func);
    uint32 SubscribeOnMessageArrived(uint8 messageType, OnMessageArrived func);
    void UnsubscrieOnMessageArrived(uint32);
    void UnsubscribeAllOnMessageArrived();

    bool Send(IMessage& message);
    bool Send(uint32 userId, IMessage& message);
    void Update();
    void SetDefaultMessageConverter(MessageFormat format);

protected:
    void DisconnectUser(uint32 id);

protected:
    Utils::Time::CTimeMeasurer timeMeasurer_;
    std::unique_ptr<ISDLNetWrapper> sdlNetWrapper_;
    std::vector<std::unique_ptr<IMessageConverter>> messageConverters_;

    Sender sender_;
    Receiver receiver_;
    ConectContext context_;
    ConnectionManager connectionManager_;

    ServerCreator serverCreator_;
    ClientCreator clientCreator_;

    std::unordered_map<uint8, std::vector<std::pair<uint32, OnMessageArrived>>> onMessageArrivedSubcribes_;
    std::vector<DisconectFunc> disconnectSubscribes_;

    bool running_;

    uint32 idPool_;
};
}  // namespace Network
