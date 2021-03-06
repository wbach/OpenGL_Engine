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
        , textMessagesCount_{10}
    {
        CLogger::Instance().ImmeditalyLog();

        testShortMessage_.clear();
        for (uint32 x = 0; x < TEXT_MSG_ARRAY_SIZE - 1; ++x)
        {
            auto c = static_cast<char>(x);
            if (c == '"' or c == 0)
            {
                c = '\'';
            }
            testShortMessage_.push_back(c);
        }
        DEBUG_LOG("testShortMessage_ : " +  testShortMessage_);
        DEBUG_LOG("testShortMessage_ size : " + std::to_string(testShortMessage_.size()));
    }

    void ClientMain()
    {
        isConnected_ = clientGateway_.ConnectToServer("baszek", "haslo", "127.0.0.1", 1991);

        if (not isConnected_)
        {
            isRunning_.store(false);
            return;
        }

        for (uint32 i = 0; i < textMessagesCount_; ++i)
        {
            TextMessage textMessage(testShortMessage_);
            DEBUG_LOG("TextMessage textMessage size : " + std::to_string(textMessage.GetText().size()));
            clientGateway_.Send(textMessage);
        }
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
            auto t           = textMessage->GetText();
            DEBUG_LOG("Server recevied message : " + t + " size : " + std::to_string(t.size()));
            receviedMessage_ = textMessage->GetText();

            if (testShortMessage_ == receviedMessage_)
            {
                DEBUG_LOG("==");
            }
            else {
                DEBUG_LOG("!=");
            }
            ++textMessagesRecvCount_;
            if (textMessagesRecvCount_ >= textMessagesCount_)
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

    }
    void Start()
    {
        textMessagesRecvCount_ = 0;
        serverStarted_         = false;
        isConnected_           = false;
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

    uint32 textMessagesCount_;
    uint32 textMessagesRecvCount_;
};

TEST_F(ClientServerTests, FirstConnectionXmlConverter)
{
    serverGateway_.SetDefaultMessageConverterFormat(MessageFormat::Xml);
    clientGateway_.SetDefaultMessageConverterFormat(MessageFormat::Xml);
    Start();
    EXPECT_TRUE(isConnected_);
    EXPECT_EQ(testShortMessage_, receviedMessage_);
}

TEST_F(ClientServerTests, FirstConnectionBinaryConverter)
{
    serverGateway_.SetDefaultMessageConverterFormat(MessageFormat::Binary);
    clientGateway_.SetDefaultMessageConverterFormat(MessageFormat::Binary);
    Start();
    EXPECT_TRUE(isConnected_);
    EXPECT_EQ(testShortMessage_, receviedMessage_);
}


}  // namespace Network
