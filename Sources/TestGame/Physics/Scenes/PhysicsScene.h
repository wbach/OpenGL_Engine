#pragma once
#include <atomic>
#include <map>
#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Hero/Stats.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "GameEngine/Scene/Scene.hpp"
#include "TestGame/Single/PlayerInputController.h"

class CPlayer;
namespace GameEngine
{
class GameObject;
}  // namespace GameEngine

enum class CameraType
{
    FirstPerson,
    ThridPerson
};

namespace PhysicsTestGame
{
typedef std::unordered_map<GameEngine::TerrainTextureType, std::string> TerrainTexturesFilesMap;

class PhysicsScene : public GameEngine::Scene
{
public:
    PhysicsScene();
    virtual ~PhysicsScene() override;
    virtual int Initialize() override;
    virtual void PostInitialize() override
    {
    }
    virtual int Update(float deltaTime) override;

private:
    void AddTerrain(const TerrainTexturesFilesMap& textures);
    std::unique_ptr<GameEngine::GameObject> CreateGameObjectInstance(float scale, const vec2& position, bool isDynamic = false);
    void CreateAndAddGameEntity(const std::string& filename, float scale, const vec2& position, uint32_t textureIndex = 0, bool isDynamic = false);
    void AddBoxes(const vec3& pos);
    void AddExampleMesh(const vec3& pos, float scale);
    void UpdateObjectsCountText();
    void AddDebuxBoxesPlane(const vec2& offset);
    void RemoveObjectsUnderYValue(float y);
    void KeyOperations();
    template <typename Shape>
    void AddPhysicObject(const std::string& modelFilename, const vec3& pos, const vec3& shapePositionOffset, const vec3& dir, float scale, bool isStatic);

private:
    std::vector<GameEngine::GameObject*> objects_;
    std::vector<GameEngine::GameObject*> terrains_;
};
}  // namespace PhysicsTestGame
