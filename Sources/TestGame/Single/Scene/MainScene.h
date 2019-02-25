#pragma once
#include <map>
#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Hero/Stats.h"
#include "GameEngine/Scene/Scene.hpp"
#include "TestGame/Single/PlayerInputController.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include <optional>
#include <functional>

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

typedef std::unordered_map<GameEngine::TerrainTextureType, std::string> TerrainTexturesFilesMap;

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
    void AddTerrain(const TerrainTexturesFilesMap& textures, const glm::vec3& position);
    std::vector<float> CreateGrassPositions(GameEngine::GameObject*, vec2);

    std::unique_ptr<GameEngine::GameObject> CreateGameObjectInstance(float scale, const vec2& position, bool isDynamic = false);
    std::unique_ptr<GameEngine::GameObject> CreateGameObjectInstance(const std::string& name, float scale, const vec2& position, bool isDynamic = false);
    void CreateAndAddGameEntity(const std::string& filename, float scale, const vec2& position, uint32_t textureIndex = 0, bool isDynamic = false);
    template <typename Shape>
    void AddPhysicObject(const std::string& modelFilename, const vec3& pos, const vec3& shapePositionOffset, const vec3& dir, float scale, bool isStatic);
    void CreateExmapleStrtupObject();

private:
    std::shared_ptr<common::Controllers::CharacterController> characterController_;
    std::shared_ptr<PlayerInputController> playerInputController_;
    common::Hero::CommonStats playerStats_;
    common::Transform lookAtCameraTransform_;

    double timeClock = 0;
    float deltaTime;

    CameraType camType;
    GameEngine::GameObject* player;
    std::unique_ptr<GameEngine::Components::TerrainHeightGetter> terrainHeightGetter_;

private:
    vec3 centerObjectPosition_;
    GameEngine::Light* pointLight_;
    GameEngine::GameObject* lightBulb_;
};
