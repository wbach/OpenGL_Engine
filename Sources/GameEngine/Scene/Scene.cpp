#include "Scene.hpp"
#include "../Renderers/GUI/GuiRenderer.h"
#include "../Input/InputManager.h"
#include "Logger/Log.h"

namespace GameEngine
{
	Scene::Scene(const std::string& name)
		: name(name)
		, directionalLight(vec3(10000, 15000, 10000), vec3(0.8))
		, camera(new CCamera)
		, gui_(nullptr)
		, inputManager_(nullptr)
	{
	}

	Scene::~Scene()
	{
		Log(__FUNCTION__);
		if (inputManager_ != nullptr)
		{
			inputManager_->UnsubscribeAll();
		}
	}

	void Scene::FullUpdate(float deltaTime)
	{
		Update(deltaTime);
		
		if (inputManager_ != nullptr)
		{
			inputManager_->ProcessKeysEvents();
		}
	}

	CGameObject* Scene::AddGameObject(CGameObject* object, const vec3& position)
	{
		object->worldTransform.SetPosition(position);
		gameObjects.emplace_back(object);
		return gameObjects.back().get();
	}

	void Scene::SetAddSceneEventCallback(AddEvent func)
	{
		addSceneEvent = func;
	}

	std::list<CGameObject*> Scene::GetObjectInRange(const vec3 & position, float range)
	{
		//int x = static_cast<uint>(position.x / OBJECT_GRID_SIZE);
		//int y = static_cast<uint>(position.z / OBJECT_GRID_SIZE);

		return std::list<CGameObject*>();// m_ObjectInGrid[x + y*OBJECT_GRID_COUNT];
	}

	CCamera* Scene::GetCamera()
	{
		return camera.get();
	}

	const CLight & Scene::GetDirectionalLight() const
	{
		return directionalLight;
	}

	const std::vector<CLight>& Scene::GetLights() const
	{
		return lights;
	}
} // GameEngine