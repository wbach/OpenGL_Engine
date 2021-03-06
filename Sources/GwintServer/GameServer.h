#pragma once
#include <Gwint/GameLine.h>
#include <Gwint/Messages/Messages.h>
#include <Gwint/Player.h>
#include <Input/InputManager.h>
#include <SDL2/SDL_net.h>
#include <Display/DisplayManager.hpp>
#include <map>

struct GamePlayer
{
    Player player;
    std::string deck_xml;
    uint32 swaped_cards_at_start = 0;
    bool accepted_deck           = false;  // download deck
    bool accepted_enemy_deck     = false;
    bool have_cards_in_hand      = false;
    bool end_choosing_cards      = false;  // swap cards at the start
    bool pushed_card             = false;
    bool end_move                = false;
    std::map<uint32, uint32> cards_to_swap;
    uint32 totalScore = 0;
    uint32 won_rounds = 0;

    void ResetToNextMove()
    {
        pushed_card = false;
        end_move    = false;
        totalScore  = 0;
    }

    void Reset()
    {
        player = Player();
        deck_xml.clear();
        cards_to_swap.clear();
        totalScore            = 0;
        won_rounds            = 0;
        swaped_cards_at_start = 0;
        accepted_deck         = false;  // download deck
        accepted_enemy_deck   = false;
        have_cards_in_hand    = false;
        end_choosing_cards    = false;  // swap cards at the start
        pushed_card           = false;
        end_move              = false;
    }
};

class GameServer
{
    enum class GameStates
    {
        WAIT_FOR_PLAYERS,
        SEND_DECKS,
        SEND_CARDS_IN_HAND,
        SWAP_CARDS_START,
        START_GAME,
        PLAYER_1_MOVE,
        PLAYER_2_MOVE,
        END_ROUND
    };

   public:
    GameServer();
    void Start();
    void Update();

   private:
    void StartGame();
    bool WaitForPlayers();
    bool NewGameProcedure();
    bool SendCardsInHand();
    bool SwapCardStarProcedure();
    bool StartGameProcedure();
    bool Player1Move();
    bool Player2Move();
    void ChangeState(GameStates state);
    void EndRound();
    void ClearGameLines();

    void SendCardsInHand(uint32 player_index);
    void WaitForAkceptDeckPlayer(uint32 player_index);
    void PushCard(uint32 who, uint32 i);
    void SwapCardsPlayer(uint32 player_index);
    void AddPushCard(const GwentMessages::PushCardMessage& msg, uint32 player_index);
    GwentMessages::ScoreMessage PrepareScoreMsg(uint32 player);

    CInputManager m_InputManager;
    CDisplayManager display;

    std::map<LineTypes, GameLine> GameLines[2];

    GamePlayer player[2];

    GameStates current_state = GameStates::WAIT_FOR_PLAYERS;

    uint32 player_start_game = 3;  //
};
