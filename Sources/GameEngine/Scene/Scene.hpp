#pragma once
#include "Types.h"
#include "SceneEvents.h"
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Renderers/GUI/GuiContext.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Time/DayNightCycle.h"
#include <list>
#include <memory>
#include <vector>
#include <atomic>
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

	typedef  std::unordered_map< uint32, std::unique_ptr<CGameObject>> GameObjects;

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
		void RemoveGameObject(CGameObject* object);
		void SetAddSceneEventCallback(AddEvent func);

		// GetObjects
		std::list<CGameObject*> GetObjectInRange(const vec3& position, float range);
		inline const GameObjects& GetGameObjects();

		// Cameras
		ICamera* GetCamera();
		void SetCamera(std::shared_ptr<ICamera> camera);

		// Lights
		const Light& GetDirectionalLight() const;
		const std::vector<Light>& GetLights() const;
		inline const DayNightCycle& GetDayNightCycle() const;
		inline float GetGlobalTime();

		inline CResourceManager& GetResourceManager();

		void CreateResourceManger(IGraphicsApiPtr graphicsPtr);
		void SetInputManager(InputManager* input);
		void SetRenderersManager(Renderer::RenderersManager* manager);
		void SetDisplayManager(CDisplayManager* displayManager);
		void SetPhysicsApi(Physics::IPhysicsApiPtr physicsApi);

		template<class T>
		T* AddComponent(CGameObject* obj)
		{
			auto comp = componentFactory_.Create(T::type, obj);
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
		Physics::IPhysicsApiPtr physicsApi_;

		// Minimum one light on scene only (night - decrease strength)
		float gloabalTime = 0.f;
		Light directionalLight;
		std::vector<Light> lights;
		DayNightCycle dayNightCycle;

		std::shared_ptr<ICamera> camera;
	//	std::mutex cameraMutex;

		GameObjects gameObjects;

		Time time_;
		std::shared_ptr<CResourceManager> resourceManager_;
		Components::ComponentController componentController_;
		Components::ComponentFactory componentFactory_;

		std::atomic_bool simulatePhysics_;
	};

	inline const GameObjects& Scene::GetGameObjects()
	{
		return gameObjects;
	}

	inline const DayNightCycle& Scene::GetDayNightCycle() const
	{
		return dayNightCycle;
	}

	// Resources
	inline CResourceManager& Scene::GetResourceManager()
	{
		return *resourceManager_;
	}

	inline float Scene::GetGlobalTime()
	{
		return gloabalTime;
	}

	inline void Scene::CreateResourceManger(IGraphicsApiPtr graphicsPtr)
	{
		resourceManager_ = std::make_shared<CResourceManager>(graphicsPtr);
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
	inline void Scene::SetPhysicsApi(Physics::IPhysicsApiPtr physicsApi)
	{
		physicsApi_ = physicsApi;
	}
} // GameEngine