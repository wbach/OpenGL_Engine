#include <gtest/gtest.h>
#include "../UtilsNetwork/Gateway.h"

#include <UtilsNetwork/Messages/TextMessage.h>
#include <condition_variable>
#include <mutex>

namespace Network
{
class ClientServerTests : public ::testing::Test
{
   public:
    ClientServerTests()
        : isConnected_{false}
    {
    }

    void ClientMain()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        cv.wait(mlock, [&]() { return serverStarted_; });

        DEBUG_LOG("Started");

        isConnected_ = clientGateway_.ConnectToServer("baszek", "haslo", "127.0.0.1", 1991);

        if (not isConnected_)
        {
            isRunning_.store(false);
            return;
        }

        TextMessage textMessage("test text message. Hello World!");
        clientGateway_.Send(textMessage);
        isRunning_.store(false);
    }

    void ServerMain()
    {
        serverGateway_.StartServer(30, 1991, [&]() {
            DEBUG_LOG("Callback");
            std::lock_guard<std::mutex> lk(mutex_);
            serverStarted_ = true;
            cv.notify_all();
        });

        while (isRunning_.load())
        {
            serverGateway_.Update();
        }

        DEBUG_LOG("Server closed.");
    }

    void SetUp() override
    {
        isRunning_.store(true);
        clientThread_ = std::thread([&]() { ClientMain(); });
        ServerMain();
    }
    void TearDown() override
    {
        clientThread_.join();
    }

   protected:
    Gateway serverGateway_;
    Gateway clientGateway_;
    std::thread clientThread_;
    std::atomic_bool isRunning_;

    bool serverStarted_;
    bool isConnected_;

    std::mutex mutex_;
    std::condition_variable cv;
};

TEST_F(ClientServerTests, FirstConnection)
{
    EXPECT_TRUE(isConnected_);
}

}  // namespace Network
