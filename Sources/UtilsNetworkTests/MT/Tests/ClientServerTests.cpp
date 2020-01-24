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
        , testShortMessage_{"test text message. Hello World!"}
    {
    }

    void ClientMain()
    {
        isConnected_ = clientGateway_.ConnectToServer("baszek", "haslo", "127.0.0.1", 1991);

        if (not isConnected_)
        {
            isRunning_.store(false);
            return;
        }

        TextMessage textMessage(testShortMessage_);
        clientGateway_.Send(textMessage);
    }

    bool StartServer()
    {
        auto serverStarted_ = serverGateway_.StartServer(30, 1991);

        if (not serverStarted_)
        {
            return false;
        }

        serverGateway_.SubscribeOnMessageArrived(MessageTypes::Text, [&](auto, std::unique_ptr<IMessage> imessage) {
            auto textMessage = castMessageAs<TextMessage>(imessage.get());
            DEBUG_LOG("Server recevied message : " + textMessage->GetText());
            receviedMessage_ = textMessage->GetText();
            isRunning_.store(false);
        });

        return true;
    }

    void ServerMain()
    {
        while (isRunning_.load())
        {
            serverGateway_.Update();
        }

        DEBUG_LOG("Server closed.");
    }

    void SetUp() override
    {
        serverStarted_ = false;
        isConnected_   = false;
        isRunning_.store(true);

        if (StartServer())
        {
            clientThread_ = std::thread([&]() { ClientMain(); });
            ServerMain();
        }
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

    std::string receviedMessage_;
    std::string testShortMessage_;
};

TEST_F(ClientServerTests, FirstConnectionBinaryConverter)
{
    serverGateway_.SetDefaultMessageConverterFormat(MessageFormat::Binary);
    clientGateway_.SetDefaultMessageConverterFormat(MessageFormat::Binary);
    EXPECT_TRUE(isConnected_);
    EXPECT_EQ(testShortMessage_, receviedMessage_);
}

TEST_F(ClientServerTests, FirstConnectionXmlConverter)
{
    EXPECT_TRUE(isConnected_);
    EXPECT_EQ(testShortMessage_, receviedMessage_);
}

}  // namespace Network
