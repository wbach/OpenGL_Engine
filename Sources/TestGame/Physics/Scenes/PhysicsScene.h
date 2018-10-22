#pragma once
#include <atomic>
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
class Terrain;
}  // namespace GameEngine

enum class CameraType
{
    FirstPerson,
    ThridPerson
};

namespace PhysicsTestGame
{
class PhysicsScene : public GameEngine::Scene
{
public:
    PhysicsScene();
    virtual ~PhysicsScene() override;
    virtual int Initialize() override;
    virtual void PostInitialize() override{}
    virtual int Update(float deltaTime) override;

private:
    GameEngine::Terrain* AddTerrain(GameEngine::TerrainTexturesMap& textures, const glm::vec3&);
    GameEngine::GameObject* CreateGameObject(float scale, const vec3& position, bool isDynamic = false);
    void AddBoxes(const vec3& pos);
    void AddBarrel(const vec3& pos);
    void AddBox(const vec3& pos, const vec3& dir, float scale, bool isStatic = false);
    void AddSphere(const vec3& pos, const vec3& dir, float scale, bool isStatic = false);
    void UpdateObjectsCountText();
    void AddDebuxBoxesPlane(const vec2& offset);
    void RemoveObjectsUnderYValue(float y);
    void KeyOperations();

private:
    std::vector<GameEngine::GameObject*> objects_;
    std::vector<GameEngine::GameObject*> terrains_;
};
}  // PhysicsTestGame
