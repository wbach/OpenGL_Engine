#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../GameEngine/Engine/Debuger.h"
#include "../GameEngine/Objects/ObjectBuilder.h"
#include "../../Characters/NetworkCharacter.h"
#include "../../MrpgGameContext.h"
#include "../UtilsNetwork/Gateway.h"
#include "../../../../UtilsNetwork/Messages/GetCharacterData/GetCharacterDataMsgResp.h"
#include <unordered_map>

//namespace Network
//{
//	class CGateway;
//} // Network

namespace MmmoRpg
{
	class PlayerController;

	class MainRpgScene : public CScene
	{
	public:
		MainRpgScene(Network::CGateway& gateway, MrpgGameContext& gameContext);
		virtual ~MainRpgScene() override;
		virtual int		Initialize() override;
		virtual void	PostInitialize() override {};
		virtual int		Update(float deltaTime) override;

	private:
		void InitGui();
		void UpdatePlayerandCamera(float time);
		void CheckCollisions();
		void AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position);
		TerrainTexturesMap CreateTerrainTexturesMap();
		std::vector<float> CreateGrassPositions(CGameObject*);

	private:
		void ReqNetworkSceneCharacters();
		void WaitForNetworkCharacters();
		void CheckIncomingMessages();
		void HandleNetworkCharacterMsg(std::shared_ptr<Network::GetCharacterDataMsgResp> msg);
		void ControlPlayer();

	private:
		std::vector<CGameObject*> terrains;
		double timeClock = 0;
		float deltaTime;
		std::unordered_map<uint32, std::shared_ptr<NetworkCharacter>> networkCharacters_;
		Network::CGateway& gateway_;
		MrpgGameContext& gameContext_;
		std::shared_ptr<PlayerController> playerController_;
	};
} // MmmoRpg