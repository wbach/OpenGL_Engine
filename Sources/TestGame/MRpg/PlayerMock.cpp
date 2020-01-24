#include "PlayerMock.h"
#include <Common/Messages/GetCharacterData/GetCharacterDataMsgReq.h>
#include <Common/Messages/GetCharacterData/GetCharacterDataMsgResp.h>
#include <Common/Messages/GetCharacterData/GetCharactersDataMsgReq.h>
#include <Common/Messages/GetCharacters/GetCharactersMsgReq.h>
#include <Common/Messages/GetCharacters/GetCharactersMsgResp.h>
#include <Common/Messages/MessageTypes.h>
#include <Common/Messages/SelectCharacter/SelectCharacterMsgReq.h>
#include <Common/Messages/SelectCharacter/SelectCharacterMsgResp.h>
#include <Common/Messages/TransformMessages/TransformMsgReq.h>
#include <Common/Messages/TransformMessages/TransformMsgResp.h>
#include <Logger/Log.h>
#include <Time/TimeMeasurer.h>
#include <UtilsNetwork/Gateway.h>
#include "NetworkTypes.h"

using namespace Network;

namespace Mock
{
struct IMockState
{
    IMockState(Gateway& gateway)
        : gateway_(gateway)
        , isFinished_(false)
    {
    }

    virtual ~IMockState()
    {
    }
    virtual void OnInit() = 0;
    void FullUpdate()
    {
        gateway_.Update();
        Update();
    }
    virtual void Update() = 0;
    bool IsFinished()
    {
        return isFinished_;
    };

    Timepoint sentTime_;
    Gateway& gateway_;
    bool isFinished_;
};

struct GetCharacterState : public IMockState
{
    GetCharacterState(Gateway& gateway, std::optional<uint32>& selectedId)
        : IMockState(gateway)
        , selectedId_(selectedId)
    {
        onMessageSubscribeId = gateway_.SubscribeOnMessageArrived(common::MessageTypes::GetCharactersResp, std::bind(&GetCharacterState::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
    }
    virtual ~GetCharacterState() override
    {
        gateway_.UnsubscrieOnMessageArrived(onMessageSubscribeId);
    }
    virtual void OnInit() override
    {
        DEBUG_LOG("");
        GetCharacters();
        sentTime_ = std::chrono::high_resolution_clock::now();
    }
    virtual void Update() override
    {
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - sentTime_).count() > 5)
        {
            DEBUG_LOG("timeout...");
            OnInit();
        }
    }

    void GetCharacters()
    {
        DEBUG_LOG("");
        gateway_.Send(common::GetCharactersMsgReq());
    }

    void OnMessage(Network::UserId, std::unique_ptr<Network::IMessage> message)
    {
        DEBUG_LOG("");
        if (message->GetType() == common::MessageTypes::GetCharactersResp)
        {
            GetCharacter(*static_cast<const common::GetCharactersMsgResp*>(message.get()));
            DEBUG_LOG(message->ToString());
        }
    }

    void GetCharacter(const common::GetCharactersMsgResp& character)
    {
        for (auto& data : character.characterInfo)
        {
            if (data)
            {
                selectedId_ = data->id_;
                isFinished_ = true;
                return;
            }
        }
    }
    uint32 onMessageSubscribeId = 0;
    std::optional<uint32>& selectedId_;
};

struct SelectCharacterState : public IMockState
{
    SelectCharacterState(Gateway& gateway, std::optional<uint32>& selectedId)
        : IMockState(gateway)
        , selectedId_(selectedId)
    {
        onMessageSubscribeId = gateway_.SubscribeOnMessageArrived(common::MessageTypes::SelectCharacterResp, std::bind(&SelectCharacterState::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
    }
    virtual ~SelectCharacterState() override
    {
        gateway_.UnsubscrieOnMessageArrived(onMessageSubscribeId);
    }
    virtual void OnInit() override
    {
        DEBUG_LOG("");
        SelectCharacters();
        sentTime_ = std::chrono::high_resolution_clock::now();
    }
    virtual void Update() override
    {
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - sentTime_).count() > 5)
        {
            DEBUG_LOG("timeout...");
            OnInit();
        }
    }

    void SelectCharacters()
    {
        DEBUG_LOG("");
        common::SelectCharacterMsgReq selectCharacterMsgReq;
        selectCharacterMsgReq.id = *selectedId_;
        gateway_.Send(selectCharacterMsgReq);
    }

    void OnMessage(Network::UserId, std::unique_ptr<Network::IMessage> message)
    {
        if (message->GetType() == common::MessageTypes::SelectCharacterResp)
        {
            SelectCharacter(*static_cast<common::SelectCharacterMsgResp*>(message.get()));
            DEBUG_LOG(message->ToString());
        }
    }

    void SelectCharacter(const common::SelectCharacterMsgResp& character)
    {
        if (character.status_ == common::MessageStatus::Ok && selectedId_.value() == character.id)
        {
            isFinished_ = true;
        }
    }
    uint32 onMessageSubscribeId = 0;
    std::optional<uint32>& selectedId_;
};

struct GetCharacterDataState : public IMockState
{
    GetCharacterDataState(Gateway& gateway, std::optional<uint32>& selectedId)
        : IMockState(gateway)
        , selectedId_(selectedId)
    {
        onMessageSubscribeId = gateway_.SubscribeOnMessageArrived(common::MessageTypes::GetCharacterDataResp, std::bind(&GetCharacterDataState::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
    }
    virtual ~GetCharacterDataState() override
    {
        gateway_.UnsubscrieOnMessageArrived(onMessageSubscribeId);
    }
    virtual void OnInit() override
    {
        DEBUG_LOG("");
        GetData();
        sentTime_ = std::chrono::high_resolution_clock::now();
    }
    virtual void Update() override
    {
        // Log("");
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - sentTime_).count() > 5)
        {
            DEBUG_LOG("timeout...");
            OnInit();
        }
    }

    void GetData()
    {
        DEBUG_LOG("");
        common::GetCharactersDataMsgReq getCharacterDataMsgReq;
        getCharacterDataMsgReq.mapId = 1;
        gateway_.Send(getCharacterDataMsgReq);
    }

    void OnMessage(Network::UserId, std::unique_ptr<IMessage> message)
    {
        DEBUG_LOG("");
        if (message->GetType() == common::MessageTypes::GetCharacterDataResp)
        {
            GetDataCharacter(*static_cast<common::GetCharacterDataMsgResp*>(message.get()));
            DEBUG_LOG(message->ToString());
        }
    }

    void GetDataCharacter(const common::GetCharacterDataMsgResp& character)
    {
        if (selectedId_.value() == character.networkCharcterId)
        {
            isFinished_ = true;
        }
    }
    uint32 onMessageSubscribeId = 0;
    std::optional<uint32>& selectedId_;
};

struct GameSceneState : public IMockState
{
    GameSceneState(Gateway& gateway, std::optional<uint32>& selectedId)
        : IMockState(gateway)
        , selectedId_(selectedId)
        , timer_(60, false)
    {
        onMessageSubscribeId = gateway_.SubscribeOnMessageArrived(common::MessageTypes::TransformResp, std::bind(&GameSceneState::OnMessage, this, std::placeholders::_1,  std::placeholders::_2));
        timer_.AddOnTickCallback(std::bind(&GameSceneState::PrintPing, this));
    }
    virtual ~GameSceneState() override
    {
        gateway_.UnsubscrieOnMessageArrived(onMessageSubscribeId);
    }
    void PrintPing()
    {
        std::cout << "Ping : " << std::to_string(ping) << "ms" << std::endl;
    }
    virtual void OnInit() override
    {
        SendTransform();
        sentTime_ = std::chrono::high_resolution_clock::now();
    }
    virtual void Update() override
    {
        timer_.StartFrame();
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - sentTime_).count() > 50)
        {
            OnInit();
            // Log("Next transform message : " + std::to_string(forwardBackward) + ", push? : " +
            // std::to_string(pushMessage) );
        }
        timer_.EndFrame();
    }

    void SendTransform()
    {
        common::TransformMsgReq getCharacterDataMsgReq;
        getCharacterDataMsgReq.action = pushMessage ? common::TransformAction::PUSH : common::TransformAction::POP;
        getCharacterDataMsgReq.type   = (forwardBackward < 2) ? common::TransformMessageTypes::MOVE_FORWARD : common::TransformMessageTypes::MOVE_BACKWARD;
        getCharacterDataMsgReq.id     = *selectedId_;
        gateway_.Send(getCharacterDataMsgReq);
        pushMessage = not pushMessage;

        ++forwardBackward;

        if (forwardBackward > 3)
            forwardBackward = 0;

        sentTransformTime_ = std::chrono::high_resolution_clock::now();
        ;
    }

    void OnMessage(Network::UserId, std::unique_ptr<IMessage> message)
    {
//        if (message.second->GetType() == MessageTypes::GetCharacterDataResp)
//        {
//            DEBUG_LOG("Got GetCharacterDataResp.");
//        }
        if (message->GetType() == common::MessageTypes::TransformResp)
        {
            auto m = castMessageAs<common::TransformMsgResp>(message.get());
            if (m->id == *selectedId_)
            {
                auto now = std::chrono::high_resolution_clock::now();
                auto t   = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - sentTransformTime_).count());
                ping += static_cast<int>(t);
                ping /= 2;

                DEBUG_LOG("Got TransformResp, server response time : " + std::to_string(t) + "ms");
            }
        }
    }

    int forwardBackward = 0;
    bool pushMessage    = true;
    uint32 ping         = 0;
    uint32 onMessageSubscribeId = 0;
    std::optional<uint32>& selectedId_;
    Timepoint sentTransformTime_;
    Utils::Time::CTimeMeasurer timer_;
};

class PlayerMock
{
public:
    PlayerMock()
    {
        InitScenario();
    }
    void Start()
    {
        if (!ConnectToServer())
        {
            DEBUG_LOG("Cannot connect to server. Name: " + name);
            return;
        }

        if (!scenario_.empty())
        {
            scenario_.front()->OnInit();
        }

        while (!scenario_.empty())
        {
            scenario_.front()->FullUpdate();

            if (scenario_.front()->IsFinished())
            {
                scenario_.pop_front();

                if (!scenario_.empty())
                {
                    scenario_.front()->OnInit();
                }
            }
        }
    }
    bool ConnectToServer()
    {
        std::string host = "127.0.0.1";
        DEBUG_LOG(" " + host);

        for (uint32 x = 100; x < 200; ++x)
        {
            name = "mock_" + std::to_string(x);
            if (gateway.ConnectToServer(name, "haslo", host, 1991))
            {
                return true;
            }
        }
        return false;
    }
    void InitScenario()
    {
        // clang-format off
        scenario_ = {std::make_shared<GetCharacterState>(gateway, seletedChracterId),
                     std::make_shared<SelectCharacterState>(gateway, seletedChracterId),
                     std::make_shared<GetCharacterDataState>(gateway, seletedChracterId),
                     std::make_shared<GameSceneState>(gateway, seletedChracterId)};
        // clang-format on
    }

private:
    std::list<std::shared_ptr<IMockState>> scenario_;
    uint32 mockId = 100;
    std::optional<uint32> seletedChracterId;
    Gateway gateway;
    std::string name;
    Timepoint sentTime_;
};

void StartMock()
{
    PlayerMock mock;
    mock.Start();
}
}  // namespace Mock
