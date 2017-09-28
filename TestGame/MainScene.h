#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../GameEngine/Engine/Debuger.h"
#include "../GameEngine/Objects/ObjectBuilder.h"
#include "Player.h"
#include <map>

class CPlayer;
class CEngine;

class MainScene : public CScene
{
public:
	MainScene(CEngine& engine);
	virtual ~MainScene() override;
	virtual int		Initialize();
	virtual void	PostInitialize() {};
	virtual int		Update();

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
	CEngine& engine;
	CDebuger debuger;
};
