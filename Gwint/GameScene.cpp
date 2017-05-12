#include "GameScene.h"
#include "States/ChooseCardState.h"
#include "States/PlayerMoveState.h"
#include "States/EnemyState.h"
#include "Renderer.h"
#include <Camera/FirstPersonCamera.h>
#include <Renderers/GUI/GuiRenderer.h>
#include <Engine/Engine.h>
#include <Renderers/GUI/Text/GuiText.h>

GwintGameScene::GwintGameScene(CEngine & engine)
	: engine(engine)
	, renderer(nullptr)
	, quit(false)
{
	m_State = std::make_unique<CChooseCardState>(player, &engine.m_InputManager, GameLines);

	GameLines =
	{
		{ LineTypes::CLOSE_COMBAT, GameLine(glm::vec3(-.225f, 0.015, .63f)) },
		{ LineTypes::ARCHEER, GameLine(glm::vec3(-.225f, -0.225, .63f)) },
		{ LineTypes::BALIST, GameLine(glm::vec3(-.225f, -0.48, .63f)) }
	};

	EnemyGameLines =
	{
		{ LineTypes::CLOSE_COMBAT, GameLine(glm::vec3(-.225f, 0.26, .63f)) },
		{ LineTypes::ARCHEER, GameLine(glm::vec3(-.225f, 0.51, .63f)) },
		{ LineTypes::BALIST, GameLine(glm::vec3(-.225f, 0.76, .63f)) }
	};

	InitGui();
}

GwintGameScene::~GwintGameScene() 
{
	quit = true;
	m_NetworkMsssagesThread.join();
}

int GwintGameScene::Initialize()
{
	m_Camera = std::make_unique<CFirstPersonCamera>(&engine.m_InputManager, &engine.m_DisplayManager);
	m_Camera->SetYaw(0);
	m_Camera->SetPitch(0);
	m_Camera->UpdateViewMatrix();

	m_NetworkMsssagesThread = std::thread(&GwintGameScene::NetworkCheckMessages, this);

	return 0;
}

int GwintGameScene::Update()
{
	if (renderer == nullptr)
		return 0;

	NetworkProcerdure();

	renderer->cards.clear();

	RenderScores();

	RenderLine(GameLines);
	RenderLine(EnemyGameLines);	

	CheckState();

	return 0;
}

void GwintGameScene::RenderLine(const std::map<LineTypes, GameLine>& line) const
{
	for (const auto& line : line)
	{
		RenderCards(renderer->cards, line.second.GetCards(), table_card_size);
	}
}

void GwintGameScene::RenderCards(std::list<SCard>& target, const std::vector<SCard>& cards, const glm::vec3 & scale) const
{
	for (const auto& card : cards)
	{
		target.push_back(card);
		target.back().scale = scale;
	}
}

void GwintGameScene::RenderScores()
{
	//TotalScorePlayer = TotalScoreEnemy = 0;
	//for (auto& line : GameLines)
	//	TotalScorePlayer += line.second.CalculateStrengthLine();
	//for (auto& line : EnemyGameLines)
	//	TotalScoreEnemy += line.second.CalculateStrengthLine();

	guiText->texts["Sum_p1"].text = std::to_string(TotalScorePlayer);
	guiText->texts["Sum_e1"].text = std::to_string(TotalScoreEnemy);


	guiText->texts["Line_p1"].text = std::to_string(GameLines[LineTypes::CLOSE_COMBAT].strength);
	guiText->texts["Line_p2"].text = std::to_string(GameLines[LineTypes::ARCHEER].strength);
	guiText->texts["Line_p3"].text = std::to_string(GameLines[LineTypes::BALIST].strength);

	guiText->texts["Line_e1"].text = std::to_string(EnemyGameLines[LineTypes::CLOSE_COMBAT].strength);
	guiText->texts["Line_e2"].text = std::to_string(EnemyGameLines[LineTypes::ARCHEER].strength);
	guiText->texts["Line_e3"].text = std::to_string(EnemyGameLines[LineTypes::BALIST].strength);

	guiText->texts["PWon"].text = "Wons : " + std::to_string(network_round_wons);
	guiText->texts["EWon"].text = "Wons : " + std::to_string(network_eround_wons);

	guiText->texts["PlayerHandCardsCount"].text = "Cards: " + std::to_string(ps_cardsInHand);
	guiText->texts["PlayerDeckCardsCount"].text = "Deck: " + std::to_string(ps_cardInDeck);
	guiText->texts["EnemyHandCardsCount"].text = "Cards: " + std::to_string(es_CardsInHand);
	guiText->texts["EnemyDeckCardsCount"].text = "Deck: " + std::to_string(es_cardInDeck);
}

void GwintGameScene::CheckState()
{
	if (m_State == nullptr ||  currentNetworState == NetworkState::END_GAME)
		return;

	m_State->KeyInputDir();

	auto state_return = m_State->KeyInputRetrun();

	if (state_return == GameStates::END_ROUND)
	{
		std::cout << "New State! " << std::endl;
		std::cout << "Cards in deck : " << player.cards_in_deck.size() << "\n";
		std::cout << "Enemy Cards in deck : " << enemy.cards_in_deck.size() << "\n";

		auto current_State = m_State->m_GameStateType;
		m_State.reset();

		if (current_State == GameStates::CHOOSE_CARDS_START)
		{
			
		}
		else if (current_State == GameStates::PLAYER_ROUND)
		{
			m_State = std::make_unique<CEnemyState>(player, currentNetworState);
		}
		else if (current_State == GameStates::ENEMY_ROUND)
		{
			m_State = std::make_unique<CPlayerMoveState>(player, &engine.m_InputManager, GameLines);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));		
	}	

	for (const auto& card : m_State->CardRender())
	{
		renderer->cards.push_back(card);
	}
}

void GwintGameScene::InitGui()
{
	CGUIRenderer* gui_renderer = new CGUIRenderer();
	guiText = new CGuiText("../Data/GUI/consola.ttf", engine.m_Projection.GetWindowSize());
	gui_renderer->AddElement(guiText);

	SGuiTextElement gui_text;
	gui_text.isActive = false;
	gui_text.text = "Test";
	gui_text.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);

	gui_text.position = glm::vec2(-0.55, -0.0);
	guiText->texts["Line_p1"] = gui_text;
	gui_text.position = glm::vec2(-0.55, -0.3);
	guiText->texts["Line_p2"] = gui_text;
	gui_text.position = glm::vec2(-0.55, -0.6);
	guiText->texts["Line_p3"] = gui_text;

	gui_text.position = glm::vec2(-0.75, -0.15);
	guiText->texts["Sum_p1"] = gui_text;

	gui_text.position = glm::vec2(-0.75, 0.4);
	guiText->texts["Sum_e1"] = gui_text;

	gui_text.position = glm::vec2(-0.55, 0.25);
	guiText->texts["Line_e1"] = gui_text;
	gui_text.position = glm::vec2(-0.55, 0.55);
	guiText->texts["Line_e2"] = gui_text;
	gui_text.position = glm::vec2(-0.55, 0.825);
	guiText->texts["Line_e3"] = gui_text;

	gui_text.position = glm::vec2(-0.95, -0.75);
	gui_text.text = "PlayerHandCardsCount";
	guiText->texts["PlayerHandCardsCount"] = gui_text;

	gui_text.position = glm::vec2(0.70, -0.825);
	gui_text.text = "PlayerDeckCardsCount";
	guiText->texts["PlayerDeckCardsCount"] = gui_text;

	gui_text.position = glm::vec2(0.70, -0.625);
	gui_text.text = "Won : 0";
	guiText->texts["PWon"] = gui_text;

	gui_text.position = glm::vec2(-0.95, 0.75);
	gui_text.text = "EnemyHandCardsCount";
	guiText->texts["EnemyHandCardsCount"] = gui_text;

	gui_text.position = glm::vec2(0.7, 0.35);
	gui_text.text = "Won : 0";
	guiText->texts["EWon"] = gui_text;

	gui_text.position = glm::vec2(0.7, 0.825);
	gui_text.text = "EnemyDeckCardsCount";
	guiText->texts["EnemyDeckCardsCount"] = gui_text;

	gui_text.position = glm::vec2(-0.3, -0.25);
	gui_text.text = "Pass";
	guiText->texts["PlayerPass"] = gui_text;

	gui_text.position = glm::vec2(-0.3, 0.25);
	gui_text.text = "Enemy Pass";
	guiText->texts["EnemyPass"] = gui_text;

	gui_text.position = glm::vec2(-0.3, 0);
	gui_text.text = "Info Text here";
	gui_text.isActive = true;
	guiText->texts["InfoMessage"] = gui_text;

	engine.m_Renderers.emplace_back(gui_renderer);
	gui_renderer->Init();
}

void GwintGameScene::ShowScores(bool show)
{
	guiText->texts["Line_p1"].isActive = show;
	guiText->texts["Line_p2"].isActive = show;
	guiText->texts["Line_p3"].isActive = show;
	guiText->texts["Sum_p1"].isActive = show;
	guiText->texts["Sum_e1"].isActive = show;
	guiText->texts["Line_e1"].isActive = show;
	guiText->texts["Line_e2"].isActive = show;
	guiText->texts["Line_e3"].isActive = show;

	guiText->texts["PlayerHandCardsCount"].isActive = show;
	guiText->texts["PlayerDeckCardsCount"].isActive = show;
	guiText->texts["EnemyHandCardsCount"].isActive = show;
	guiText->texts["EnemyDeckCardsCount"].isActive = show;

	guiText->texts["PWon"].isActive = show;
	guiText->texts["EWon"].isActive = show;
}

void GwintGameScene::ClearGameLines()
{
	GameLines.clear();
	EnemyGameLines.clear();

	GameLines =
	{
		{ LineTypes::CLOSE_COMBAT, GameLine(glm::vec3(-.225f, 0.015, .63f)) },
		{ LineTypes::ARCHEER, GameLine(glm::vec3(-.225f, -0.225, .63f)) },
		{ LineTypes::BALIST, GameLine(glm::vec3(-.225f, -0.48, .63f)) }
	};

	EnemyGameLines =
	{
		{ LineTypes::CLOSE_COMBAT, GameLine(glm::vec3(-.225f, 0.26, .63f)) },
		{ LineTypes::ARCHEER, GameLine(glm::vec3(-.225f, 0.51, .63f)) },
		{ LineTypes::BALIST, GameLine(glm::vec3(-.225f, 0.76, .63f)) }
	};
	;
}

void GwintGameScene::EndGame()
{
	guiText->texts["InfoMessage"].text = end_text;

	auto msg = SDLClientGetway::Instance().GetMessage();

	if (msg.empty())
		return;


}

void GwintGameScene::NetworkCheckMessages()
{
	while (!quit)
	{
		SDLClientGetway::Instance().CheckIncomingMessage();
		SDLClientGetway::Instance().CheckComplexMessage();
	}
}

void GwintGameScene::NetworkProcerdure()
{
	switch (currentNetworState)
	{
	case NetworkState::DOWNLOAD_DECKS:
		NetworkDownloadDeck();
		break;
	case NetworkState::RAND_START_CARDS:
		NetworkGetCardsInHand();
		break;
	case NetworkState::SWAP_CARDS:
		NetworkSwapCards();
		break;
	case NetworkState::WAIT_FOR_START:
		NetworkWaitForStart();
		break;
	case NetworkState::PLAYER_TURN:
		NetworkPlayerState();		
		break;
	case NetworkState::ENEMY_TURN:
		NetworEnemyState();
		break;
	case NetworkState::END_GAME:
		EndGame();
		break;
	}
}

void GwintGameScene::NetworkDownloadDeck()
{
	guiText->texts["InfoMessage"].text = "Waiting for deck...";

	std::string deck;

	auto response = SDLClientGetway::Instance().GetMessage();
	if (!response.empty())
	{
		std::cout << response << std::endl;

		auto set = GameCards::Instance().LoadCards(DeckType::NORTH, response);
		if (set)
		{
			SDLClientGetway::Instance().SendMessage("DECK_OK");

			GameCards::Instance().LoadTexturesInCards(engine.m_Scene->GetResourceManager(), true);

			for (const auto& decks : GameCards::Instance().GetCards())
			{
				for (const auto& card : decks.second)
				{
					player.cards_in_deck.push_back(card);
					//player.cards_in_hand.push_back(card);
				}
			}
			currentNetworState = NetworkState::RAND_START_CARDS;
			SDLClientGetway::Instance().ClearMessagesQueue();
		}
	}	
}
void GwintGameScene::NetworkGetCardsInHand()
{
	guiText->texts["InfoMessage"].text = "Waiting for rand cards...";

	auto msg = SDLClientGetway::Instance().GetMessage();

	if (msg.empty())
		return;

	if (msg == "BEGIN_XML_MESSAGE")
		return;

	if (msg == "END_XML_MESSAGE")
	{
		SDLClientGetway::Instance().SendMessage("HAND_CARDS_OK");
		SDLClientGetway::Instance().ClearMessagesQueue();
		currentNetworState = NetworkState::SWAP_CARDS;
		return;
	}

	Log(msg);

	GwentMessages::SwapCardMessage card_msg;
	if (card_msg.Serialized(msg))
	{
		SCard card;
		int index_to_erase = 0;
		for (const auto& c : player.cards_in_deck)
		{
			if (c.name == card_msg.card_name)
			{
				card = c;
				break;
			}
			index_to_erase++;
		}

		//auto card = player.cards_in_deck[card_msg.index];
	
		if (card.name == card_msg.card_name)
		{
			player.cards_in_hand.push_back(card);
			player.cards_in_deck.erase(player.cards_in_deck.begin() + index_to_erase);
		}
		else
		{
			Error("Founded card in deck is diffrent than in message. " + card.name + " != " + card_msg.card_name);
		}
	}
}
void GwintGameScene::NetworkSwapCards()
{
	guiText->texts["InfoMessage"].isActive = false;

	auto message = SDLClientGetway::Instance().GetMessage();
	if (!message.empty())
	{
		GwentMessages::SwapCardMessage response;
		if (response.Serialized(message))
		{
			Log("Get response : " + message);
			if (response.type == GwentMessages::SwapCardMessage::MessageType::RESPONSE)
			{
				auto swaped_card = player.cards_in_deck[response.index];
				auto card = player.cards_in_deck[response.prev_index];
				player.cards_in_hand[response.prev_index] = swaped_card;
				player.cards_in_deck[response.index] = card;
				player.swaped_cards++;
				SDLClientGetway::Instance().SendMessage("SWAP_CARD_OK");
			}
			else
			{
				Log("Swap card : Except response message but income request msg.");
			}
		}
		else if (message == "END_SWAPING_CARDS")
		{
			currentNetworState = NetworkState::WAIT_FOR_START;
			SDLClientGetway::Instance().ClearMessagesQueue();
			ShowScores(true);
			Log("Swaped cards is end.");
			Log("Wait for start.");

			m_State = std::make_unique<CEnemyState>(player, currentNetworState);
		}
		else
		{
			Log("Swap card : Except response message but income other type message.");
		}
	}

}
void GwintGameScene::NetworkWaitForStart()
{
	guiText->texts["InfoMessage"].isActive = true;
	guiText->texts["InfoMessage"].text = "Waiting for start Game...";

	guiText->texts["EnemyPass"].isActive = false;
	guiText->texts["PlayerPass"].isActive = false;

	ClearGameLines();

	auto msg = SDLClientGetway::Instance().GetMessage();

	if (msg.empty())
		return;

	Log("Recv msg : \n" + msg);

	GwentMessages::StartGameMessage start_msg;
	if (start_msg.Serialized(msg))
	{
		if (start_msg.playerToStart == GwentMessages::Player::PLAYER)
		{
			currentNetworState = NetworkState::PLAYER_TURN;
			SDLClientGetway::Instance().ClearMessagesQueue();
			m_State = std::make_unique<CPlayerMoveState>(player, &engine.m_InputManager, GameLines);
		}
		else
		{
			SDLClientGetway::Instance().ClearMessagesQueue();
			currentNetworState = NetworkState::ENEMY_TURN;
			m_State = std::make_unique<CEnemyState>(player, currentNetworState);
		}
	}
}



void GwintGameScene::NetworEnemyState()
{
	guiText->texts["InfoMessage"].isActive = true;
	guiText->texts["InfoMessage"].text = "Waiting for enemy move...";

	auto msg = SDLClientGetway::Instance().GetMessage();

	GwentMessages::PushCardMessage	push_card_msg;
	GwentMessages::ScoreMessage		score_msg;

	if (!msg.empty())
	{
		Log("Recv : " + msg);
		if (msg == "START_MOVE")
		{
			m_State = std::make_unique<CPlayerMoveState>(player, &engine.m_InputManager, GameLines);
			currentNetworState = NetworkState::PLAYER_TURN;
		}
		else if(msg == "ENEMY_PASS")
		{
			m_State = std::make_unique<CPlayerMoveState>(player, &engine.m_InputManager, GameLines);
			currentNetworState = NetworkState::PLAYER_TURN;
			guiText->texts["EnemyPass"].isActive = true;
		}
		else if (msg == "END_ROUND")
		{
			currentNetworState = NetworkState::WAIT_FOR_START;
			return;
		}
		else if (msg == "YOU_WON")
		{
			end_text = "YOU WIN !!!!!!!!!";
			currentNetworState = NetworkState::END_GAME;
			m_State = std::make_unique<CChooseCardState>(player, &engine.m_InputManager, GameLines);
			return;
		}
		else if (msg == "YOU_LOSE")
		{
			end_text = "YOU LOSE ;(";			
			currentNetworState = NetworkState::END_GAME;
			m_State = std::make_unique<CChooseCardState>(player, &engine.m_InputManager, GameLines);
			return;
		}
		else if (push_card_msg.Serialized(msg))
		{
			Log("Enemy push card msg.\n" + msg);
			Log("Card name : " + push_card_msg.card_name);

			if (!push_card_msg.texturePath.empty())
			{
				SCard card;
				card.texture_path = push_card_msg.texturePath;
				card.name = push_card_msg.card_name;
				card.texture = m_ResourceManager.GetTextureLaoder().LoadTextureImmediately(push_card_msg.texturePath);
				EnemyGameLines[static_cast<LineTypes>(push_card_msg.type)].AddCard(card);

				es_CardsInHand = push_card_msg.cardsLeftInHand;
				es_cardInDeck = push_card_msg.cardsLeftInDeck;

			}
			

			//auto card = enemy.cards_in_hand[push_card_msg.index];
			//if (card.name == push_card_msg.card_name)
			//{
			//	EnemyGameLines[card.type].AddCard(card);
			//	//enemy.cards_in_hand.erase(enemy.cards_in_hand.begin() + push_card_msg.index);
			//}
			//else
			//{
			//	Error("Enemy push card network msg error.");
			//}
		}
		else if (score_msg.Serialized(msg))
		{
			Log("Score update.\n" + msg);

			TotalScoreEnemy = score_msg.scoreEnemy[GwentMessages::ScoreMessage::Total];
			TotalScorePlayer = score_msg.scorePlayer[GwentMessages::ScoreMessage::Total];

			GameLines[LineTypes::CLOSE_COMBAT].strength = score_msg.scorePlayer[GwentMessages::ScoreMessage::Line1];
			GameLines[LineTypes::ARCHEER].strength = score_msg.scorePlayer[GwentMessages::ScoreMessage::Line2];
			GameLines[LineTypes::BALIST].strength = score_msg.scorePlayer[GwentMessages::ScoreMessage::Line3];

			EnemyGameLines[LineTypes::CLOSE_COMBAT].strength = score_msg.scoreEnemy[GwentMessages::ScoreMessage::Line1];
			EnemyGameLines[LineTypes::ARCHEER].strength = score_msg.scoreEnemy[GwentMessages::ScoreMessage::Line2];
			EnemyGameLines[LineTypes::BALIST].strength = score_msg.scoreEnemy[GwentMessages::ScoreMessage::Line3];

			network_round_wons = score_msg.wonRounds[0];
			network_eround_wons = score_msg.wonRounds[1];
		}
	}
}
void GwintGameScene::NetworkPlayerState()
{
	guiText->texts["InfoMessage"].isActive = false;

	auto message = SDLClientGetway::Instance().GetMessage();

	if (!message.empty())
	{
		Log("Get message in player state: \n" + message);

		if (message == "END_MOVE")
		{
			m_State = std::make_unique<CEnemyState>(player, currentNetworState);
			currentNetworState = NetworkState::ENEMY_TURN;
			return;
		}
		else if (message == "PASS_OK")
		{
			m_State = std::make_unique<CEnemyState>(player, currentNetworState);
			currentNetworState = NetworkState::ENEMY_TURN;
			guiText->texts["PlayerPass"].isActive = true;
		}
		else if (message == "END_ROUND")
		{
			currentNetworState = NetworkState::WAIT_FOR_START;
			return;
		}
		else if (message == "YOU_WON")
		{
			end_text = "YOU WIN !!!!!!!!!";
			currentNetworState = NetworkState::END_GAME;
			m_State = std::make_unique<CChooseCardState>(player, &engine.m_InputManager, GameLines);
			return;
		}
		else if (message == "YOU_LOSE")
		{
			end_text = "YOU LOSE ;(";
			currentNetworState = NetworkState::END_GAME;
			m_State = std::make_unique<CChooseCardState>(player, &engine.m_InputManager, GameLines);
			return;
		}

		GwentMessages::PushCardMessage response;
		GwentMessages::ScoreMessage	   score_msg;

		if (response.Serialized(message))
		{
			Log("Serialized response : " + message);

			SCard card;
			int index = -1;
			int i = 0;
			for (const auto& c : player.cards_in_hand)
			{
				if (c.name == response.card_name)
				{
					card = c;
					index = i;
					break;
				}
				i++;
			}
			if (index >= 0)
			{
				GameLines[card.type].AddCard(card);
				player.cards_in_hand.erase(player.cards_in_hand.begin() + index);
				SDLClientGetway::Instance().SendMessage("PUSH_CARD_OK");
				ps_cardsInHand = response.cardsLeftInHand;
				ps_cardInDeck = response.cardsLeftInDeck;
			}			
		}
		else if (score_msg.Serialized(message))
		{
			Log("Score update.\n" + message);

			TotalScoreEnemy = score_msg.scoreEnemy[GwentMessages::ScoreMessage::Total];
			TotalScorePlayer = score_msg.scorePlayer[GwentMessages::ScoreMessage::Total];

			GameLines[LineTypes::CLOSE_COMBAT].strength = score_msg.scorePlayer[GwentMessages::ScoreMessage::Line1];
			GameLines[LineTypes::ARCHEER].strength = score_msg.scorePlayer[GwentMessages::ScoreMessage::Line2];
			GameLines[LineTypes::BALIST].strength = score_msg.scorePlayer[GwentMessages::ScoreMessage::Line3];

			EnemyGameLines[LineTypes::CLOSE_COMBAT].strength = score_msg.scoreEnemy[GwentMessages::ScoreMessage::Line1];
			EnemyGameLines[LineTypes::ARCHEER].strength = score_msg.scoreEnemy[GwentMessages::ScoreMessage::Line2];
			EnemyGameLines[LineTypes::BALIST].strength = score_msg.scoreEnemy[GwentMessages::ScoreMessage::Line3];

			network_round_wons = score_msg.wonRounds[0];
			network_eround_wons = score_msg.wonRounds[1];
		}
		else
		{
			Log("Push card : Except response message but income other type message: \n" + message);
		}
	}
	
}
//
//void GwintGameScene::NetworkWaitForStart()
//{
//	std::string msg;
//	std::cout << "Waiting for start game..." << std::endl;
//	while (1)
//	{
//		auto response = SDLClientGetway::Instance().GetMessage(msg);
//		if (response)
//		{
//			std::cout << msg << std::endl;
//
//			if (msg == "START_GAME")
//				break;
//		}
//	}
//}
//
//void GwintGameScene::NetworkGetCardsInHand()
//{
//	bool wait_for_cards = true;
//	while (wait_for_cards)
//	{
//		std::string message;
//		if (SDLClientGetway::Instance().GetSingleMessage(message))
//		{
//			auto pos = message.find("BEGIN_XML_MESSAGE");
//			if (pos != std::string::npos)
//			{
//				message = message.substr(pos);
//
//				while (1)
//				{
//					Log(message);
//
//					if (!SDLClientGetway::Instance().GetSingleMessage(message))
//						continue;
//
//					Log("Dwonload cards in hand : " + message);
//
//					if (message == "END_XML_MESSAGE")
//					{
//						SDLClientGetway::Instance().SendMessage("HAND_CARDS_OK");
//						wait_for_cards = false;
//						break;
//					}
//					GwentMessages::SwapCardMessage msg;
//					if (msg.Serialized(message))
//					{
//						auto card = scene->player.cards_in_deck[msg.index];
//
//						if (card.name == msg.card_name)
//						{
//							scene->player.cards_in_hand.push_back(card);
//							scene->player.cards_in_deck.erase(scene->player.cards_in_deck.begin() + msg.index);
//						}
//						else
//						{
//							Error("Founded card in deck is diffrent than in message. " + card.name + " != " + msg.card_name);
//						}
//					}
//				}
//			}
//		}
//	}
//}
