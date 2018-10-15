#pragma once
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Objects/ObjectBuilder.h"
#include "Common/Controllers/CharacterController/Character.h"
#include "TestGame/Single/PlayerInputController.h"
#include "Common/Hero/Stats.h"
#include <map>
#include <atomic>

class CPlayer;
class CGameObject;
class CTerrain;

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
		virtual int		Initialize() override;
		virtual void	PostInitialize() override {};
		virtual int		Update(float deltaTime) override;

		
	private:
		CTerrain* AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position);
		CGameObject* CreateGameObject(float scale, const vec3& position, bool isDynamic = false);
		void AddBoxes(const vec3& pos);
		void AddBarrel(const vec3& pos);
		void AddBox(const vec3& pos, const vec3& dir, float scale, bool isStatic = false);
		void AddSphere(const vec3& pos, const vec3& dir, float scale, bool isStatic = false);
		void UpdateObjectsCountText();
		void AddDebuxBoxesPlane(const vec2& offset);
		void RemoveObjectsUnderYValue(float y);
		void KeyOperations();

	private:
		std::vector<CGameObject*> objects_;
		std::vector<CGameObject*> terrains_;
	};
} // PhysicsTestGame
