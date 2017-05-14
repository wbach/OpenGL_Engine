#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "Player.h"
#include <map>

class CPlayer;
class CEngine;
struct STerrain;

class MainScene : public CScene
{
public:
    MainScene(CEngine& engine);
    virtual ~MainScene() override;
    virtual int		Initialize();
    virtual void	PostInitialize() {};
    virtual int		Update();
private:
    std::map<STerrain::TexturesTypes, std::string> CreateTerrainTexturesMap();
    void AddTerrain(std::map<STerrain::TexturesTypes, std::string>& textures, const glm::vec3& position);
    void AddGrass();
    CPlayer* player;
    STerrain *terrain;
    double time_clock = 0;
    CEngine& engine;
};
