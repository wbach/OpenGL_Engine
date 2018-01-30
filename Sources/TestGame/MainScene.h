#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../GameEngine/Engine/Debuger.h"
#include "../GameEngine/Objects/ObjectBuilder.h"
#include "Player.h"
#include <map>

class CPlayer;

class MainScene : public CScene
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
	void ReloadShadersInRenderer();
	void DebugRenderOptionsControl();
	void CheckCollisions();
	void AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position);
	TerrainTexturesMap CreateTerrainTexturesMap();
	std::vector<float> CreateGrassPositions(CGameObject*);

private:
	CPlayer* player;
	std::vector<CGameObject*> terrains;
	double timeClock = 0;
	CDebuger debuger;
	float deltaTime;
};
