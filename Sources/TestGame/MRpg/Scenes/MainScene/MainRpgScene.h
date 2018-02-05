#pragma once
#include "Gateway.h"
#include "../MRpgScene.h"
#include "Objects/ObjectBuilder.h"
#include "TestGame/MRpg/Characters/NetworkCharacter.h"
#include "Common/Controllers/CharacterController/CharacterActions.h"
#include "Messages/TransformMessages/TransformMsgResp.h"
#include <unordered_map>


namespace Network
{
	class GetCharacterDataMsgResp;
} // Network

namespace MmmoRpg
{
	class PlayerController;

	class MainRpgScene : public MRpgScene
	{
	public:
		MainRpgScene(Network::CGateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext);
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
		void Dispatch(const Network::BoxMessage& msg);
		void HandleNetworkCharacterMsg(std::shared_ptr<Network::GetCharacterDataMsgResp> msg);
		void HandleTransformMsg(std::shared_ptr<Network::TransformMsgResp> msg);

	private:
		float deltaTime;
		double timeClock = 0;
		std::vector<CGameObject*> terrains;
		std::unordered_map<uint32, std::shared_ptr<NetworkCharacter>> networkCharacters_;
		std::shared_ptr<PlayerController> playerController_;
	};
} // MmmoRpg