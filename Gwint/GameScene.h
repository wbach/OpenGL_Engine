#pragma once
#include "Player.h"
#include "GameLine.h"
#include "Network/NetworkStates.h"
#include "States/State.h"
#include <Scene/Scene.hpp>
#include <map>
#include <atomic>

class CState;
class CGuiText;
class CEngine;
class CGwitRenderer;

class GwintGameScene : public CScene
{
public:
	GwintGameScene(CEngine& engine);
	virtual ~GwintGameScene() override;
	virtual int		Initialize() override;
	virtual void	PostInitialize() override {};
	virtual int		Update() override;

	CGwitRenderer* renderer;
	std::map<LineTypes, GameLine> GameLines;
	std::map<LineTypes, GameLine> EnemyGameLines;
	uint TotalScorePlayer, TotalScoreEnemy;

	Player player;
	Player enemy;
private:
	void RenderLine(const std::map<LineTypes, GameLine>& line) const;
	void RenderCards(std::list<SCard>& target, const std::vector<SCard>& cards, const glm::vec3& scale) const;
	void RenderScores();
	void CheckState();
	void InitGui();
	void ShowScores(bool show);	
	void ClearGameLines();
	void EndGame();

	uint ps_cardsInHand, ps_cardInDeck, es_CardsInHand, es_cardInDeck;
	uint network_round_wons, network_eround_wons;
	std::string end_text;

	CGuiText* guiText;
	CEngine& engine;
	std::unique_ptr<CState> m_State;
	std::atomic_bool quit;
	std::thread m_NetworkMsssagesThread;	

	NetworkState currentNetworState;

	void NetworkCheckMessages();
	void NetworkProcerdure();
	void NetworkDownloadDeck();
	void NetworkGetCardsInHand();
	void NetworkSwapCards();
	void NetworkWaitForStart();
	void NetworEnemyState();
	void NetworkPlayerState();
};