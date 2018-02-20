#include "PlayerMock.h"
#include "Gateway.h"
#include "Messages/GetCharacters/GetCharactersMsgReq.h"
#include "Messages/GetCharacters/GetCharactersMsgResp.h"
#include "Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "Messages/SelectCharacter/SelectCharacterMsgResp.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgReq.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgResp.h"
#include "Messages/GetCharacterData/GetCharactersDataMsgReq.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include "Messages/TransformMessages/TransformMsgResp.h"
#include "NetworkTypes.h"
#include "Messages/MessageTypes.h"
#include "Time/TimeMeasurer.h"
#include "Logger/Log.h"

using namespace Network;

namespace Mock
{
	struct IMockState
	{
		IMockState(CGateway& gateway)
			: gateway_(gateway)
			, isFinished_(false)
		{}

		virtual ~IMockState() {}
		virtual void OnInit() = 0;
		void FullUpdate()
		{
			gateway_.Update();
			Update();
		}
		virtual void Update() = 0;
		bool IsFinished() { return isFinished_; };

		Timepoint sentTime_;
		CGateway& gateway_;
		bool isFinished_;
	};

	struct GetCharacterState : public IMockState
	{
		GetCharacterState(CGateway& gateway, wb::optional<uint32>& selectedId)
			: IMockState(gateway)
			, selectedId_(selectedId)
		{
			gateway_.SubscribeOnMessageArrived("GetCharactersMsgReq", std::bind(&GetCharacterState::OnMessage, this, std::placeholders::_1));
		}
		virtual ~GetCharacterState() 
		{
			gateway_.UnsubscrieOnMessageArrived("GetCharactersMsgReq");
		}
		virtual void OnInit() override
		{
			Log("");
			GetCharacters();
			sentTime_ = std::chrono::high_resolution_clock::now();
		}
		virtual void Update() override
		{
			auto now = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(now - sentTime_).count() > 5)
			{
				Log("timeout...");
				OnInit();
			}
		}

		void GetCharacters()
		{
			Log("");
			auto getCharactersMsgReq = std::make_unique<GetCharactersMsgReq>();
			gateway_.Send(getCharactersMsgReq.get());
		}

		void OnMessage(const BoxMessage& message)
		{
			Log("");
			if (message.second->GetType() == MessageTypes::GetCharactersResp)
			{
				GetCharacter(*castMessageAs<GetCharactersMsgResp>(message.second.get()));
				Log(message.second->ToString());
			}
		}

		void GetCharacter(const GetCharactersMsgResp& character)
		{
			for (auto& data : character.characterInfo)
			{
				if (data)
				{
					selectedId_ = data.constValue().id_;
					isFinished_ = true;
					return;
				}
			}
		}
		wb::optional<uint32>& selectedId_;
	};

	struct SelectCharacterState : public IMockState
	{
		SelectCharacterState(CGateway& gateway, wb::optional<uint32>& selectedId)
			: IMockState(gateway)
			, selectedId_(selectedId)
		{
			gateway_.SubscribeOnMessageArrived("SelectCharacterState", std::bind(&SelectCharacterState::OnMessage, this, std::placeholders::_1));
		}
		virtual ~SelectCharacterState()
		{
			gateway_.UnsubscrieOnMessageArrived("SelectCharacterState");
		}
		virtual void OnInit() override
		{
			Log("");
			SelectCharacters();
			sentTime_ = std::chrono::high_resolution_clock::now();
		}
		virtual void Update() override
		{
			auto now = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(now - sentTime_).count() > 5)
			{
				Log("timeout...");
				OnInit();
			}
		}

		void SelectCharacters()
		{
			Log("");
			auto selectCharacterMsgReq = std::make_unique<SelectCharacterMsgReq>();
			selectCharacterMsgReq->id = selectedId_.constValue();
			gateway_.Send(selectCharacterMsgReq.get());
		}

		void OnMessage(const BoxMessage& message)
		{
			if (message.second->GetType() == MessageTypes::SelectCharacterResp)
			{
				SelectCharacter(*castMessageAs<SelectCharacterMsgResp>(message.second.get()));
				Log(message.second->ToString());
			}
		}

		void SelectCharacter(const SelectCharacterMsgResp& character)
		{
			if (character.status_ == MessageStatus::Ok && selectedId_.value() == character.id)
			{				
				isFinished_ = true;
			}
		}
		wb::optional<uint32>& selectedId_;
	};

	struct GetCharacterDataState : public IMockState
	{
		GetCharacterDataState(CGateway& gateway, wb::optional<uint32>& selectedId)
			: IMockState(gateway)
			, selectedId_(selectedId)
		{
			gateway_.SubscribeOnMessageArrived("GetCharacterDataState", std::bind(&GetCharacterDataState::OnMessage, this, std::placeholders::_1));
		}
		virtual ~GetCharacterDataState()
		{
			gateway_.UnsubscrieOnMessageArrived("GetCharacterDataState");
		}
		virtual void OnInit() override
		{
			Log("");
			GetData();
			sentTime_ = std::chrono::high_resolution_clock::now();
		}
		virtual void Update() override
		{
			//Log("");
			auto now = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(now - sentTime_).count() > 5)
			{
				Log("timeout...");
				OnInit();
			}
		}

		void GetData()
		{
			Log("");
			auto getCharacterDataMsgReq = std::make_unique<GetCharactersDataMsgReq>();
			getCharacterDataMsgReq->mapId = 1;
			gateway_.Send(getCharacterDataMsgReq.get());
		}

		void OnMessage(const BoxMessage& message)
		{
			Log("");
			if (message.second->GetType() == MessageTypes::GetCharacterDataResp)
			{
				GetDataCharacter(*castMessageAs<GetCharacterDataMsgResp>(message.second.get()));
				Log(message.second->ToString());
			}
		}

		void GetDataCharacter(const GetCharacterDataMsgResp& character)
		{			
			if (selectedId_.value() == character.networkCharcterId)
			{
				isFinished_ = true;
			}
		}
		wb::optional<uint32>& selectedId_;
	};

	struct GameSceneState : public IMockState
	{
		GameSceneState(CGateway& gateway, wb::optional<uint32>& selectedId)
			: IMockState(gateway)
			, selectedId_(selectedId)
			, timer_(60, false)
		{
			gateway_.SubscribeOnMessageArrived("GameSceneState", std::bind(&GameSceneState::OnMessage, this, std::placeholders::_1));
			timer_.AddOnTickCallback(std::bind(&GameSceneState::PrintPing, this));
		}
		virtual ~GameSceneState()
		{
			gateway_.UnsubscrieOnMessageArrived("GameSceneState");
		}
		void PrintPing()
		{
			std::cout << "Ping : " << std::to_string(ping) << "ms" << std::endl;
		}
		virtual void OnInit() override
		{
			//Log("");
			SendTransform();
			sentTime_ = std::chrono::high_resolution_clock::now();
		}
		virtual void Update() override
		{
			auto now = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(now - sentTime_).count() > 50)
			{
				OnInit();
				//Log("Next transform message : " + std::to_string(forwardBackward) + ", push? : " + std::to_string(pushMessage) );
			}
			timer_.CalculateAndLock();
		}

		void SendTransform()
		{
			//Log("");

			auto getCharacterDataMsgReq = std::make_unique<TransformMsgReq>();
			getCharacterDataMsgReq->action = pushMessage ? TransformAction::PUSH : TransformAction::POP;
			getCharacterDataMsgReq->type = (forwardBackward < 2) ? TransformMessageTypes::MOVE_FORWARD : TransformMessageTypes::MOVE_BACKWARD;
			getCharacterDataMsgReq->id = selectedId_.constValue();
			gateway_.Send(getCharacterDataMsgReq.get());
			pushMessage = !pushMessage;

			++forwardBackward;

			if (forwardBackward > 3)
				forwardBackward = 0;

			sentTransformTime_ = std::chrono::high_resolution_clock::now();;
		}

		void OnMessage(const BoxMessage& message)
		{
			if (message.second->GetType() == MessageTypes::GetCharacterDataResp)
			{
				Log("Got GetCharacterDataResp.");
			}
			if (message.second->GetType() == MessageTypes::TransformResp)
			{
				auto m = castMessageAs<TransformMsgResp>(message.second.get());
				if (m->id == selectedId_.constValue())
				{
					auto now = std::chrono::high_resolution_clock::now();
					auto t = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - sentTransformTime_).count());
					ping += static_cast<int>(t);
					ping /= 2;

					Log("Got TransformResp, server response time : " + std::to_string(t) + "ms");					
				}				
			}
		}

		int forwardBackward = 0;
		bool pushMessage = true;
		uint32 ping = 0;
		wb::optional<uint32>& selectedId_;
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
				Log("Cannot connect to server. Name: " + name);
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
			Log(" " + host);

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
			scenario_ =
			{
				std::make_shared<GetCharacterState>(gateway, seletedChracterId),
				std::make_shared<SelectCharacterState>(gateway, seletedChracterId),
				std::make_shared<GetCharacterDataState>(gateway, seletedChracterId),
				std::make_shared<GameSceneState>(gateway, seletedChracterId)
			};
		}


	private:
		std::list<std::shared_ptr<IMockState>> scenario_;
		uint32 mockId = 100;
		wb::optional<uint32> seletedChracterId;
		CGateway gateway;
		std::string name;
		Timepoint sentTime_;
	};

	void StartMock()
	{
		PlayerMock mock;
		mock.Start();
	}
}
