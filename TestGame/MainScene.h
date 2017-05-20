#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../GameEngine/Objects/ObjectBuilder.h"
#include "Player.h"
#include <map>

class CPlayer;
class CEngine;
//struct STerrain;

class MainScene : public CScene
{
public:
    MainScene(CEngine& engine);
    virtual ~MainScene() override;
    virtual int		Initialize();
    virtual void	PostInitialize() {};
    virtual int		Update();
private:
   TerrainTexturesMap CreateTerrainTexturesMap();
    void AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position);
    std::vector<float> CreateGrassPositions(CGameObject*);
    CPlayer* player;
	std::vector<CGameObject*> terrains;
  //  STerrain *terrain;
    double time_clock = 0;
    CEngine& engine;
};
