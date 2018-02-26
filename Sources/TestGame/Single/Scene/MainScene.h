#pragma once
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Objects/ObjectBuilder.h"
#include "Common/Controllers/CharacterController/Character.h"
#include "TestGame/Single/PlayerInputController.h"
#include "Common/Hero/Stats.h"
#include <map>

class CPlayer;
class CGameObject;

enum class CameraType
{
	FirstPerson,
	ThridPerson
};

class MainScene : public GameEngine::Scene
{
public:
	MainScene();
	virtual ~MainScene() override;
	virtual int		Initialize() override;
	virtual void	PostInitialize() override {};
	virtual int		Update(float deltaTime) override;

private:
	void InitGui();
	void UpdatePlayerandCamera(float time);
	void KeyOperations();
	void CheckCollisions(float dt);
	void AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position);
	TerrainTexturesMap CreateTerrainTexturesMap();
	std::vector<float> CreateGrassPositions(CGameObject*, vec2);
	CGameObject* AddEntity(const std::string& modelName, float scale, const vec2& position, bool isDynamic = false);

private:
	std::shared_ptr<common::Controllers::CharacterController> characterController_;
	std::shared_ptr<PlayerInputController> playerInputController_;
	common::Hero::CommonStats playerStats_;

	common::Transform lookAtCameraTransform_;

	std::vector<CGameObject*> terrains;
	double timeClock = 0;
	float deltaTime;

	CameraType camType;
	CGameObject* player;
	std::vector<CGameObject*> terrains_;
};
