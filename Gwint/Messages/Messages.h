#pragma once
#include <rapidxml-1.13/rapidxml.hpp>
#include <rapidxml-1.13/rapidxml_print.hpp>
#include <sstream>
#include <string>

namespace GwentMessages
{
	enum class Player
	{
		PLAYER,
		ENEMY
	};

	struct StartGameMessage
	{
		Player playerToStart;

		std::string ToString()
		{
			return
				"<start_game>" + std::to_string(static_cast<int>(playerToStart)) + "</start_game>";
		}

		bool Serialized(std::string message)
		{
			if (message.empty())
				return false;

			rapidxml::xml_document<> document;
			try
			{
				document.parse<0>(const_cast<char*>(message.c_str()));

				auto node = document.first_node();

				if (node != nullptr)
				{
					std::string name = node->name();
					std::string value = node->value();
					int swaped_index = -1;

					if (name == "start_game")
					{
						if (value == "0")
						{
							playerToStart = Player::PLAYER;
							document.clear();
							return true;
						}
						else if (value == "1")
						{
							playerToStart = Player::ENEMY;
							document.clear();
							return true;
						}
					}					
					document.clear();
				}
			}
			catch (rapidxml::parse_error p)
			{
				p.what();
				return false;
			}
			return false;
		}
	};
	struct SwapCardMessage
	{
		enum class MessageType
		{
			REQUEST,
			RESPONSE,
			NOT_SET
		};
		
		uint index = 0;
		uint prev_index = 0;

		std::string card_name;
		std::string prev_card_name;

		MessageType type = MessageType::NOT_SET;

		std::string ToString()
		{
			std::string type_str = "SWAP_CARD_REQ";
			if(type == MessageType::RESPONSE)
				type_str = "SWAP_CARD_RESP";

			rapidxml::xml_document<char> document;
			auto root = document.allocate_node(rapidxml::node_element, document.allocate_string(type_str.c_str()));
			auto index_node = document.allocate_node(rapidxml::node_element, "index", document.allocate_string(std::to_string(index).c_str()));
			auto name_node = document.allocate_node(rapidxml::node_element, "name", document.allocate_string(card_name.c_str()));

			auto prev_index_node = document.allocate_node(rapidxml::node_element, "prev_index", document.allocate_string(std::to_string(prev_index).c_str()));
			auto prev_name_node = document.allocate_node(rapidxml::node_element, "prev_name", document.allocate_string(prev_card_name.c_str()));

			root->append_node(index_node);
			root->append_node(name_node);
			root->append_node(prev_index_node);
			root->append_node(prev_name_node);

			document.append_node(root);

			std::stringstream message;
			message << document;
			document.clear();

			return message.str();
		}

		bool Serialized(std::string message)
		{
			if (message.empty())
				return false;

			rapidxml::xml_document<> document;
			try
			{
				document.parse<0>(const_cast<char*>(message.c_str()));

				auto node = document.first_node();

				if (node != nullptr)
				{
					std::string name = node->name();
					
					if (name == "SWAP_CARD_RESP")
						type = MessageType::RESPONSE;
					if (name == "SWAP_CARD_REQ")
						type = MessageType::REQUEST;

					if (type != MessageType::NOT_SET)
					{
						for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
						{
							name = snode->name();
							if (name == "index")
							{
								index = std::stoi(snode->value());
							}
							else if (name == "name")
							{
								card_name = snode->value();
							}
							if (name == "prev_index")
							{
								prev_index = std::stoi(snode->value());
							}
							else if (name == "prev_name")
							{
								prev_card_name = snode->value();
							}
						}
					}
					document.clear();
				}
			}
			catch (rapidxml::parse_error p)
			{
				p.what();
				return false;
			}
			return true;
		}
	};
	struct PushCardMessage 
	{
		Player player;
		std::string card_name;		
		uint index = 0;

		std::string texturePath; //for enemy
		uint type = 0;
		uint cardsLeftInHand = 0;
		uint cardsLeftInDeck = 0;

		bool operator==(const PushCardMessage& m)
		{
			return player == m.player && card_name == m.card_name && index == m.index;
		}

		std::string ToString()
		{
			rapidxml::xml_document<char> document;
			auto root = document.allocate_node(rapidxml::node_element, "push_card");
			auto index_node = document.allocate_node(rapidxml::node_element, "index", document.allocate_string(std::to_string(index).c_str()));
			auto name_node = document.allocate_node(rapidxml::node_element, "name", document.allocate_string(card_name.c_str()));
			int p = static_cast<int>(player);
			auto player_node = document.allocate_node(rapidxml::node_element, "player", document.allocate_string(std::to_string(p).c_str()));
			auto texture_node = document.allocate_node(rapidxml::node_element, "texturePath", document.allocate_string(texturePath.c_str()));
			auto type_node = document.allocate_node(rapidxml::node_element, "lineType", document.allocate_string(std::to_string(type).c_str()));
			auto left_in_deck_node = document.allocate_node(rapidxml::node_element, "leftInDeck", document.allocate_string(std::to_string(cardsLeftInDeck).c_str()));
			auto left_in_hand_node = document.allocate_node(rapidxml::node_element, "leftInHand", document.allocate_string(std::to_string(cardsLeftInHand).c_str()));

			root->append_node(index_node);
			root->append_node(name_node);
			root->append_node(player_node);
			root->append_node(texture_node);
			root->append_node(type_node);
			root->append_node(left_in_deck_node);
			root->append_node(left_in_hand_node);

			document.append_node(root);

			std::stringstream message;
			message << document;
			document.clear();

			return message.str();
		}

		bool Serialized(std::string message)
		{
			if (message.empty())
				return false;

			rapidxml::xml_document<> document;
			try
			{
				document.parse<0>(const_cast<char*>(message.c_str()));

				auto node = document.first_node();

				if (node != nullptr)
				{
					std::string name = node->name();				

					if (name == "push_card")
					{
						for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
						{
							name = snode->name();
							std::string value = snode->value();

							if (name == "index")
							{
								index = std::stoi(value);
							}
							else if (name == "name")
							{
								card_name = value;
							}
							else if (name == "player")
							{
								if (value == "0")
								{
									player = Player::PLAYER;									
								}
								else if (value == "1")
								{
									player = Player::ENEMY;									
								}
							}
							else if (name == "texturePath")
							{
								texturePath = value;
							}
							else if (name == "lineType")
							{
								type = std::stoi(value);
							}
							else if (name == "leftInDeck")
							{
								cardsLeftInDeck = std::stoi(value);
							}
							else if (name == "leftInHand")
							{
								cardsLeftInHand = std::stoi(value);
							}
						}
						document.clear();
						return true;
					}
					document.clear();
				}
				else
				{
//					Log("Not parsed xml msg : \n" + message);
				}
			}
			catch (rapidxml::parse_error p)
			{
				p.what();
				return false;
			}
			return false;
		}
	};
	struct ScoreMessage 
	{
		enum ScoreType
		{
			Line1,
			Line2,
			Line3,
			Total,
			Count
		};
		int scorePlayer[ScoreType::Count];
		int scoreEnemy[ScoreType::Count];

		uint wonRounds[2];

		ScoreMessage()
		{
			for (uint x = 0; x < Count; ++x)
			{
				scorePlayer[x] = 0;
				scoreEnemy[x] = 0;
			}
			wonRounds[0] = 0;
			wonRounds[1] = 0;

		}

		std::string ToString()
		{

			rapidxml::xml_document<char> document;
			auto root = document.allocate_node(rapidxml::node_element, "score");
			auto player_node = document.allocate_node(rapidxml::node_element, "player");

			auto node_line1 = document.allocate_node(rapidxml::node_element, "line_1", document.allocate_string(std::to_string(scorePlayer[Line1]).c_str()));
			auto node_line2 = document.allocate_node(rapidxml::node_element, "line_2", document.allocate_string(std::to_string(scorePlayer[Line2]).c_str()));
			auto node_line3 = document.allocate_node(rapidxml::node_element, "line_3", document.allocate_string(std::to_string(scorePlayer[Line3]).c_str()));
			auto node_lineT = document.allocate_node(rapidxml::node_element, "total", document.allocate_string(std::to_string(scorePlayer[Total]).c_str()));
			auto node_wond_rounds = document.allocate_node(rapidxml::node_element, "won_rounds", document.allocate_string(std::to_string(wonRounds[0]).c_str()));

			player_node->append_node(node_line1);
			player_node->append_node(node_line2);
			player_node->append_node(node_line3);
			player_node->append_node(node_lineT);
			player_node->append_node(node_wond_rounds);

			auto enemy_node = document.allocate_node(rapidxml::node_element, "enemy");
			auto node_eline1 = document.allocate_node(rapidxml::node_element, "line_1", document.allocate_string(std::to_string(scoreEnemy[Line1]).c_str()));
			auto node_eline2 = document.allocate_node(rapidxml::node_element, "line_2", document.allocate_string(std::to_string(scoreEnemy[Line2]).c_str()));
			auto node_eline3 = document.allocate_node(rapidxml::node_element, "line_3", document.allocate_string(std::to_string(scoreEnemy[Line3]).c_str()));
			auto node_elineT = document.allocate_node(rapidxml::node_element, "total", document.allocate_string(std::to_string(scoreEnemy[Total]).c_str()));
			auto node_ewond_rounds = document.allocate_node(rapidxml::node_element, "won_rounds", document.allocate_string(std::to_string(wonRounds[1]).c_str()));

			enemy_node->append_node(node_eline1);
			enemy_node->append_node(node_eline2);
			enemy_node->append_node(node_eline3);
			enemy_node->append_node(node_elineT);
			enemy_node->append_node(node_ewond_rounds);

			root->append_node(player_node);
			root->append_node(enemy_node);

			document.append_node(root);

			std::stringstream message;
			message << document;
			document.clear();

			return message.str();
		}

		void SerializePlayerScore(rapidxml::xml_node<char>* node, int (&scorePlayer)[ScoreType::Count], uint player_index)
		{
			for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
			{
				std::string name = snode->name();
				std::string value = snode->value();

				if (name == "line_1")
				{
					scorePlayer[Line1] = std::stoi(value);
				}
				else if (name == "line_2")
				{
					scorePlayer[Line2] = std::stoi(value);
				}
				else if (name == "line_3")
				{
					scorePlayer[Line3] = std::stoi(value);
				}
				else if (name == "total")
				{
					scorePlayer[Total] = std::stoi(value);
				}
				else if (name == "won_rounds")
				{
					wonRounds[player_index] = std::stoi(value);
				}
			}
		}

		bool Serialized(std::string message)
		{
			if (message.empty())
				return false;

			rapidxml::xml_document<char> document;
			try
			{
				document.parse<0>(const_cast<char*>(message.c_str()));

				auto node = document.first_node();

				if (node != nullptr)
				{
					std::string name = node->name();					

					if (name == "score")
					{
						for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
						{
							name = snode->name();
							std::string value = snode->value();

							if (name == "player")
							{
								SerializePlayerScore(snode, scorePlayer, 0);
							}
							else if (name == "enemy")
							{
								SerializePlayerScore(snode, scoreEnemy, 1);
							}						
						}
						return true;
					}
					document.clear();
				}
			}
			catch (rapidxml::parse_error p)
			{
				std::cout << p.what() << std::endl;;
				return false;
			}
			return false;
		}
	};
	struct EndRound
	{
		Player player;

		std::string ToString()
		{
			return
				"<end_round>" + std::to_string(static_cast<int>(player)) + "</end_round>";
		}
		bool Serialized(std::string message)
		{
			if (message.empty())
				return false;

			rapidxml::xml_document<> document;
			try
			{
				document.parse<0>(const_cast<char*>(message.c_str()));

				auto node = document.first_node();

				if (node != nullptr)
				{
					std::string name = node->name();
					std::string value = node->value();
					int swaped_index = -1;

					if (name == "end_round")
					{
						if (value == "0")
						{
							player = Player::PLAYER;
							document.clear();
							return true;
						}
						else if (value == "1")
						{
							player = Player::ENEMY;
							document.clear();
							return true;
						}
					}
					document.clear();
				}
			}
			catch (rapidxml::parse_error p)
			{
				p.what();
				return false;
			}
			return false;
		}
	};
	struct EndMove
	{
		Player player;

		std::string ToString()
		{
			return
				"<end_move>" + std::to_string(static_cast<int>(player)) + "</end_move>";
		}
		bool Serialized(std::string message)
		{
			if (message.empty())
				return false;

			rapidxml::xml_document<> document;
			try
			{
				document.parse<0>(const_cast<char*>(message.c_str()));

				auto node = document.first_node();

				if (node != nullptr)
				{
					std::string name = node->name();
					std::string value = node->value();
					int swaped_index = -1;

					if (name == "end_move")
					{
						if (value == "0")
						{
							player = Player::PLAYER;
							document.clear();
							return true;
						}
						else if (value == "1")
						{
							player = Player::ENEMY;
							document.clear();
							return true;
						}
					}
					document.clear();
				}
			}
			catch (rapidxml::parse_error p)
			{
				p.what();
				return false;
			}
			return false;
		}
	};
}
