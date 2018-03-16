#pragma once
#include "Types.h"
#include "SceneEvents.h"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Renderers/GUI/GuiContext.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Time/DayNightCycle.h"
#include <list>
#include <memory>
#include <vector>
//#include "Mutex.hpp"

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
		
		void Init();
		void PostInit();
		void FullUpdate(float deltaTime);
		void PostUpdate();

		const std::string& GetName() { return name; }

		// Add Entities
		void AddGameObject(CGameObject* object, const vec3& position = vec3(0.f), const vec3& rotation = vec3(0.0f));
		void SetAddSceneEventCallback(AddEvent func);

		// GetObjects
		std::list<CGameObject*> GetObjectInRange(const vec3& position, float range);
		inline const GameObjects& GetGameObjects();

		// Cameras
		CCamera* GetCamera();
		void SetCamera(std::unique_ptr<CCamera> camera);

		// Lights
		const CLight& GetDirectionalLight() const;
		const std::vector<CLight>& GetLights() const;
		inline const CDayNightCycle& GetDayNightCycle() const;
		inline float GetGlobalTime();

		inline CResourceManager& GetResourceManager();

		void SetInputManager(InputManager* input);
		void SetRenderersManager(Renderer::RenderersManager* manager);
		void SetDisplayManager(CDisplayManager* displayManager);

		template<class T>
		T* AddComponent(CGameObject* obj)
		{
			auto comp = componentFactory_.Create(T::type);
			auto r = comp.get();
			obj->AddComponent(std::move(comp));
			return static_cast<T*>(r);
		}

	public:
		uint32 objectCount;

	protected:
		virtual int Initialize() { return 0; };
		virtual void PostInitialize() {}	
		virtual int Update(float deltaTime) { return 0; };

	protected:
		std::string name;
		AddEvent addSceneEvent;

		//Renderer::Gui::GuiContext* gui_;
		InputManager* inputManager_;
		CDisplayManager* displayManager_;
		Renderer::RenderersManager* renderersManager_;

		// Minimum one light on scene only (night - decrease strength)
		float gloabalTime = 0.f;
		CLight directionalLight;
		std::vector<CLight> lights;
		CDayNightCycle dayNightCycle;

		std::unique_ptr<CCamera> camera;
	//	std::mutex cameraMutex;

		CResourceManager resourceManager;

		GameObjects gameObjects;

		Time time_;
		Components::ComponentController componentController_;
		Components::ComponentFactory componentFactory_;
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

	inline void Scene::SetInputManager(InputManager* input)
	{
		inputManager_ = input;
	}

	inline void Scene::SetRenderersManager(Renderer::RenderersManager* manager)
	{
		renderersManager_ = manager;
		componentFactory_.SetRendererManager(manager);
	}

	inline void Scene::SetDisplayManager(CDisplayManager* displayManager)
	{
		displayManager_ = displayManager;
	}
} // GameEngine