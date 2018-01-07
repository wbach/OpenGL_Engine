#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../GameEngine/Engine/Debuger.h"
#include "../GameEngine/Objects/ObjectBuilder.h"
#include "Player.h"
#include <map>

class CPlayer;
namespace GameEngine
{ 
class CEngine;
} // GameEngine

class MainScene : public CScene
{
public:
	MainScene(GameEngine::CEngine& engine);
	virtual ~MainScene() override;
	virtual int		Initialize() override;
	virtual void	PostInitialize() override {};
	virtual int		Update(float deltaTime) override;

private:
	void InitGui();
	void ReloadShadersInRenderer();
	void DebugRenderOptionsControl();
	void AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position);
	TerrainTexturesMap CreateTerrainTexturesMap();
	std::vector<float> CreateGrassPositions(CGameObject*);

private:
	CPlayer* player;
	std::vector<CGameObject*> terrains;
	double timeClock = 0;
	GameEngine::CEngine& engine;
	CDebuger debuger;
	float deltaTime;
};
