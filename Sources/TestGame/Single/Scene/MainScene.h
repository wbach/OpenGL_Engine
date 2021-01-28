#pragma once
#include <map>
#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Hero/Stats.h"
#include "GameEngine/Scene/Scene.hpp"
#include "TestGame/Single/PlayerInputController.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Resources/File.h"
#include <optional>
#include <functional>

class CPlayer;
namespace GameEngine
{
class GameObject;
} // namespace GameEngine

class MainScene : public GameEngine::Scene
{
public:
    MainScene();
    virtual ~MainScene() override;
    virtual int Initialize() override;
    virtual void PostInitialize() override{}
    virtual int Update(float deltaTime) override;

private:
    void KeyOperations();
};
