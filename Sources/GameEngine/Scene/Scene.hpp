#pragma once
#include "SceneEvents.h"
#include "../Camera/Camera.h"
#include "../Lights/Light.h"
#include "../Objects/GameObject.h"
#include "../Resources/ResourceManager.h"
#include "../Renderers/GUI/GuiContext.h"
#include "../Time/DayNightCycle.h"
#include "Types.h"
#include <list>
#include <memory>
#include <vector>

// Object in scene are in grid (one grid size)
const uint32 OBJECT_GRID_SIZE = 500;
// const int grid count y in worlds
const uint32 OBJECT_GRID_COUNT = 100;

namespace GameEngine
{
	class CDisplayManager;
	class InputManager;

	namespace Renderer
	{
		class RenderersManager;
	} // Renderer

	typedef  std::list<std::unique_ptr<CGameObject>> GameObjects;

	class Scene
	{
	public:
		Scene(const std::string& name);

		virtual ~Scene();
		virtual int Initialize() { return 0; };
		virtual void PostInitialize() {}	
		void FullUpdate(float deltaTime);

		const std::string& GetName() { return name; }

		// Add Entities
		void AddGameObject(CGameObject* object, const vec3& position = vec3(0.f));
		void SetAddSceneEventCallback(AddEvent func);

		// GetObjects
		std::list<CGameObject*> GetObjectInRange(const vec3& position, float range);
		inline const GameObjects& GetGameObjects();

		// Cameras
		CCamera* GetCamera();

		// Lights
		const CLight& GetDirectionalLight() const;
		const std::vector<CLight>& GetLights() const;
		inline const CDayNightCycle& GetDayNightCycle() const;

		// Resources
		inline CResourceManager& GetResourceManager();
		inline float GetGlobalTime();

		// GUi
		void SetGuiContext(Renderer::Gui::GuiContext* c);
		void SetInputManager(InputManager* input);
		void SetRenderersManager(Renderer::RenderersManager* manager);
		void SetDisplayManager(CDisplayManager* displayManager);

	public:
		uint32 objectCount;

	protected:
		virtual int Update(float deltaTime) { return 0; };

	protected:
		std::string name;
		AddEvent addSceneEvent;

		Renderer::Gui::GuiContext* gui_;
		InputManager* inputManager_;
		CDisplayManager* displayManager_;
		Renderer::RenderersManager* renderersManager_;

		// Minimum one light on scene only (night - decrease strength)
		float gloabalTime = 0.f;
		CLight directionalLight;
		std::vector<CLight> lights;
		CDayNightCycle dayNightCycle;

		std::unique_ptr<CCamera> camera;
		CResourceManager resourceManager;

		GameObjects gameObjects;
	};

	inline const GameObjects& Scene::GetGameObjects()
	{
		return gameObjects;
	}

	inline const CDayNightCycle& Scene::GetDayNightCycle() const
	{
		return dayNightCycle;
	}

	// Resources
	inline CResourceManager& Scene::GetResourceManager()
	{
		return resourceManager;
	}

	inline float Scene::GetGlobalTime()
	{
		return gloabalTime;
	}

	inline void Scene::SetGuiContext(Renderer::Gui::GuiContext* c)
	{
		gui_ = c;
	}

	inline void Scene::SetInputManager(InputManager* input)
	{
		inputManager_ = input;
	}

	inline void Scene::SetRenderersManager(Renderer::RenderersManager* manager)
	{
		renderersManager_ = manager;
	}

	inline void Scene::SetDisplayManager(CDisplayManager* displayManager)
	{
		displayManager_ = displayManager;
	}
} // GameEngine