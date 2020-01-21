#pragma once
#include <Types.h>

#include <Mutex.hpp>
#include <Thread.hpp>
#include <atomic>

#include "Gateway.h"

namespace Network
{
struct BoxMessage
{
    uint32 userId_;
    std::unique_ptr<IMessage> message_;
};
class BoxesGateway : public CGateway
{
public:
    BoxesGateway();
    ~BoxesGateway();
    void StartServer(uint32 maxClients, uint32 port);
    bool ConnectToServer(const std::string& username, const std::string& password, const std::string& host,
                         uint32 port);
    void CreateThread();
    void AddToOutbox(IMessage& message);
    void AddToOutbox(uint32 userId, IMessage& message);
    BoxMessage PopInBox();

private:
    void SendAllMessages();
    void MainLoop();
    void AddToInbox(std::unique_ptr<IMessage>);
    uint32 GetOutBoxSize();
    std::optional<BoxMessage> PopOutBox();
    void ClearOutbox();
    void PrintFps();

private:
    std::mutex outboxMutex_;
    std::mutex inboxMutex_;

    std::list<BoxMessage> outbox_;
    std::list<BoxMessage> inbox_;

    bool threadCreated;
    std::thread networkThread_;
    std::atomic_bool running;
};

typedef std::shared_ptr<BoxesGateway> BoxesGatewayPtr;
}  // namespace Network