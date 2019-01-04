#pragma once
#include <map>
#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Hero/Stats.h"
#include "GameEngine/Objects/ObjectBuilder.h"
#include "GameEngine/Scene/Scene.hpp"
#include "TestGame/Single/PlayerInputController.h"

class CPlayer;
namespace GameEngine
{
class GameObject;
} // namespace GameEngine

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
    virtual int Initialize() override;
    virtual void PostInitialize() override{}
    virtual int Update(float deltaTime) override;

private:
    void InitGui();
    void UpdatePlayerandCamera(float time);
    void KeyOperations();
    void CheckCollisions(float dt);
    void AddTerrain(GameEngine::TerrainTexturesMap& textures, const glm::vec3& position);
    GameEngine::TerrainTexturesMap CreateTerrainTexturesMap();
    std::vector<float> CreateGrassPositions(GameEngine::GameObject*, vec2);
    std::unique_ptr<GameEngine::GameObject> CreateGameObjectInstance(float scale, const vec2& position, bool isDynamic = false);

private:
    std::shared_ptr<common::Controllers::CharacterController> characterController_;
    std::shared_ptr<PlayerInputController> playerInputController_;
    common::Hero::CommonStats playerStats_;

    common::Transform lookAtCameraTransform_;

    double timeClock = 0;
    float deltaTime;

    CameraType camType;
    GameEngine::GameObject* player;
    std::vector<GameEngine::GameObject*> terrains_;
};
